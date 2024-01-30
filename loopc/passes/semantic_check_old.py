from __future__ import annotations

from enum import Enum, auto
import os

from loop_ast.base import *
from loop_ast.module import *
from loop_ast.stmt import *
from loop_ast.expr import *

from util.error_listener import ErrorListener
from loop_ast.base import AstNode


def perform_semantic_check(
    file: File, error_listener: ErrorListener, module: Module
) -> bool:
    SemanticCheck(file, error_listener, EnvironmentType.MODULE).visit(module)
    return not error_listener.had_error


class EnvironmentType(Enum):
    MODULE = auto()
    FUNC = auto()
    CLASS = auto()


class Environment:
    name: str
    error_listener: ErrorListener
    kind: EnvironmentType
    parent: Optional[Environment]
    defs: List[Identifier]

    def __init__(
        self,
        name: str,
        error_listener: ErrorListener,
        kind: EnvironmentType,
        parent: Optional[Environment] = None,
    ) -> None:
        self.name = name
        self.error_listener = error_listener
        self.kind = kind
        self.parent = parent
        self.defs = []

    def add_var(self, ident: Identifier, export: bool = False):
        if export and self.parent != None:
            self.error_listener.error(
                ident.pos, f"cannot export local declaration '{ident.text}'"
            )

        if ident.text.startswith("__"):
            self.error_listener.error(
                ident.pos, f"variable '{ident.text}' cannot start with '__'"
            )

        if pos := self.exists_inside(ident.text):
            self.error_listener.error(
                ident.pos, f"variable '{ident.text}' is already defined"
            )
            self.error_listener.note(pos, f"previous definition of '{ident.text}'")

        self.defs.append(ident)

    def exists_inside(self, name: str) -> Optional[SourcePosition]:
        for def_ in self.defs:
            if def_.text == name:
                return def_.pos

    def exists(self, name: str) -> Optional[SourcePosition]:
        if pos := self.exists_inside(name):
            return pos

        if self.kind != EnvironmentType.CLASS and self.parent:
            return self.parent.exists(name)

    def is_in_class(self) -> bool:
        return self.kind == EnvironmentType.CLASS or (
            self.parent.is_in_class() if self.parent else False
        )

    def is_func_name(self, name: str) -> bool:
        if self.kind == EnvironmentType.FUNC and self.name == name:
            return True

        if self.parent:
            return self.parent.is_func_name(name)

        return False


class SemanticCheck(AstVisitor):
    file: File
    error_listener: ErrorListener
    env: Environment

    def __init__(
        self,
        file: File,
        error_listener: ErrorListener,
        kind: EnvironmentType = EnvironmentType.MODULE,
    ) -> None:
        self.file = file
        self.error_listener = error_listener
        self.env = Environment("<script>", error_listener, kind)
        self.class_env = None

    def check(self, node: AstNode):
        return self.visit(node)

    def visit_ImportAsStmt(self, stmt: ImportAsStmt):
        self.env.add_var(stmt.name)

        module_path = os.path.join(os.path.dirname(self.file.path), stmt.path)

        from full_passes import full_passes

        full_passes(self.error_listener, module_path + ".loop", self.file.path)

    def visit_ImportFromStmt(self, stmt: ImportFromStmt):
        if len(stmt.names) == 0:
            self.error_listener.error(stmt.pos, "empty import statement")
            return

        for name in stmt.names:
            self.env.add_var(name)

        module_path = os.path.join(os.path.dirname(self.file.path), stmt.path)

        from full_passes import full_passes

        # TODO: Compiling builtins?
        full_passes(self.error_listener, module_path + ".loop", self.file.path)

    def visit_Module(self, module: Module):
        for stmt in module.stmts:
            self.check(stmt)

    def visit_VarDecl(self, stmt: VarDecl):
        if stmt.expr:
            self.check(stmt.expr)
        self.env.add_var(stmt.name, stmt.export)

    def visit_FuncDecl(self, stmt: FuncDecl):
        self.env.add_var(stmt.name, stmt.export)

        self.begin_scope(stmt.name.text)

        for arg in stmt.args:
            self.env.add_var(arg)

        self.check(stmt.body)

        self.end_scope()

    def visit_PrintStmt(self, stmt: PrintStmt):
        self.check(stmt.expr)

    def visit_BlockStmt(self, stmt: BlockStmt):
        self.begin_scope()
        for stmt in stmt.stmts:
            self.check(stmt)
        self.end_scope()

    def visit_IfStmt(self, stmt: IfStmt):
        self.check(stmt.condition)
        self.check(stmt.then_arm)
        if stmt.else_arm:
            self.check(stmt.else_arm)

    def visit_WhileStmt(self, stmt: WhileStmt):
        self.check(stmt.condition)
        self.check(stmt.block)

    def visit_ReturnStmt(self, stmt: ReturnStmt):
        if self.env.kind != EnvironmentType.FUNC:
            self.error_listener.error(stmt.pos, "unexpected return statement")

        if self.env.is_in_class() and self.env.is_func_name("init"):
            if stmt.expr != None or not (
                type(stmt.expr) == VarExpr and stmt.expr.name.text == "init"
            ):
                self.error_listener.error(
                    stmt.pos, "malformed return statement in the init method"
                )

        self.check(stmt.expr)

    def visit_ClassDecl(self, stmt: ClassDecl):
        self.env.add_var(stmt.name, stmt.export)

        self.begin_scope(stmt.name.text, EnvironmentType.CLASS)

        self.env.add_var(stmt.name)

        for method in stmt.methods:
            self.check(method)

        self.end_scope()

    def visit_Method(self, stmt: Method):
        self.env.add_var(stmt.name)

        self.begin_scope(stmt.name.text)

        for arg in stmt.args:
            self.env.add_var(arg)

        self.check(stmt.body)

        self.end_scope()

    def visit_ExprStmt(self, stmt: ExprStmt):
        self.check(stmt.expr)

    def visit_BinaryOp(self, expr: BinaryOp):
        self.check(expr.left)
        self.check(expr.right)

    def visit_UnaryOp(self, expr: UnaryOp):
        self.check(expr.expr)

    def visit_VarExpr(self, expr: VarExpr):
        if expr.name.text.startswith("__"):
            self.error_listener.error(
                expr.pos,
                "double underscore at the begginning is not allowed in variable names",
            )

        if expr.name.text == "this" and not self.env.is_in_class():
            self.error_listener.error(expr.pos, "this is not allowed in this scope")

        if not self.env.is_in_class() and not self.env.exists(expr.name.text):
            self.error_listener.error(
                expr.pos, f"undefined variable '{expr.name.text}'"
            )

    def visit_Assignment(self, expr: Assignment):
        self.check(expr.var)
        self.check(expr.expr)

        if not check_assignment_target(expr.var):
            self.error_listener.error(
                expr.pos,
                "invalid assignment target",
            )

    def visit_CallExpr(self, expr: CallExpr):
        self.check(expr.callee)
        for arg in expr.args:
            self.check(arg)

    def visit_GetAttrExpr(self, expr: GetAttrExpr):
        self.check(expr.obj)

    def visit_GetItemExpr(self, expr: GetItemExpr):
        self.check(expr.obj)
        for index in expr.index:
            self.check(index)

    def visit_DictionaryLiteral(self, expr: DictionaryLiteral):
        for pair in expr.entries:
            self.check(pair.key)
            self.check(pair.value)

    def visit_IntegerLiteral(self, expr: IntegerLiteral):
        pass

    def visit_BoolLiteral(self, expr: BoolLiteral):
        pass

    def visit_StringLiteral(self, expr: StringLiteral):
        pass

    def visit_NullLiteral(self, expr: NullLiteral):
        pass

    def begin_scope(self, name: str = "", kind: EnvironmentType = EnvironmentType.FUNC):
        self.env = Environment(name, self.error_listener, kind, self.env)

    def end_scope(self):
        self.env = self.env.parent


def check_assignment_target(expr: Expr) -> bool:
    match expr:
        case VarExpr() | GetAttrExpr() | GetItemExpr():
            return True
