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
    SemanticCheck(file, error_listener).check(module)
    return not error_listener.had_error


@dataclass
class Local:
    name: Identifier
    scope: int
    is_captured: bool


@dataclass
class ClassDef:
    name: Identifier


THIS_IDENT = Identifier(SourcePosition("", 0), "this", RefType.LOCAL, 0)


class Env:
    name: str
    error_listener: ErrorListener
    parent: Optional[Env]
    defs: List[Local]
    globals: List[Local]  # A dirty hack, but easy to use
    upvalues: List[Upvalue]
    scope: int

    def __init__(
        self,
        name: str,
        error_listener: ErrorListener,
        parent: Optional[Env] = None,
    ):
        self.name = name
        self.error_listener = error_listener
        self.parent = parent
        self.scope = 0 if parent is None else parent.scope + 1
        self.defs = [
            Local(THIS_IDENT, self.scope, False)
        ]  # TODO: Check this not in class.
        self.globals = []
        self.upvalues = []

    def new_block(self):
        self.scope += 1

    def end_block(self) -> List[bool]:
        assert self.scope > 0

        lst = []
        while len(self.defs):
            deff = self.defs[-1]
            if deff.scope == self.scope:
                lst.append(deff.is_captured)
                self.defs.pop()
            else:
                break

        self.scope -= 1
        return lst

    def define_var(self, name: Identifier, export: bool):
        self.check_redefinition(name)
        if export:
            self.check_export(name)

        if self.scope == 0:
            lst = self.globals
        else:
            lst = self.defs

        lst.append(Local(name, self.scope, False))

        name.ref_type = (
            RefType.LOCAL
            if self.scope > 0
            else RefType.EXPORT
            if export
            else RefType.GLOBAL
        )
        name.ref_index = len(lst) - 1

    def check_redefinition(self, name: Identifier):
        for deff in reversed(self.globals + self.defs):
            if deff.scope == self.scope and deff.name.text == name.text:
                self.error_listener.error(
                    name.pos, f"variable '{name.text}' is already defined"
                )
                self.error_listener.note(
                    deff.name.pos, f"previous definition of '{name.text}'"
                )
                break

    def check_export(self, name: Identifier):
        if self.scope != 0:
            self.error_listener.error(
                name.pos, "cannot export not a top-level definition"
            )

    def resolve(self, name: Identifier):
        if self.resolve_local(name):
            return

        if self.resolve_upvalue(name):
            return

        if self.resolve_global(name):
            return

        self.error_listener.error(name.pos, f"variable '{name.text}' is not defined")

    def resolve_local(self, name: Identifier) -> bool:
        for deff in reversed(self.defs):
            if deff.name.text == name.text:
                name.ref_type = deff.name.ref_type
                name.ref_index = deff.name.ref_index
                return True

        return False

    def resolve_upvalue(self, name: Identifier) -> bool:
        if not self.parent:
            return False

        if self.parent.resolve_local(name):
            self.parent.defs[name.ref_index].is_captured = True
            self.add_upvalue(name, True)
            return True

        if self.parent.resolve_upvalue(name):
            self.add_upvalue(name, False)
            return True

        return False

    def add_upvalue(self, name: Identifier, is_local: bool):
        name.ref_type = RefType.UPVALUE

        for i, upvalue in enumerate(self.upvalues):
            if upvalue.index == name.ref_index and upvalue.is_local == is_local:
                name.ref_index = i
                return

        self.upvalues.append(Upvalue(name.ref_index, is_local))
        name.ref_index = len(self.upvalues) - 1

        if name.ref_index > 255:
            self.error_listener.error(name.pos, "too many upvalues")

    def resolve_global(self, name: Identifier) -> bool:
        if self.parent:
            return self.parent.resolve_global(name)
        else:
            for deff in self.globals:
                if deff.name.text == name.text:
                    name.ref_type = deff.name.ref_type
                    name.ref_index = deff.name.ref_index
                    return True

        return False


class SemanticCheck(AstVisitor):
    file: File
    error_listener: ErrorListener
    env: Env
    classes: List[ClassDef]

    def __init__(self, file: File, error_listener: ErrorListener):
        self.file = file
        self.error_listener = error_listener
        self.env = Env("<script>", error_listener)
        self.classes = []

    def check(self, node: AstNode):
        self.visit(node)

    def visit_ImportAsStmt(self, stmt: ImportAsStmt):
        self.env.define_var(stmt.name, False)

        self.check_imported(stmt.path)

    def visit_ImportFromStmt(self, stmt: ImportFromStmt):
        raise Exception("should be lowered")
        """
        if len(stmt.names) == 0:
            self.error_listener.error(stmt.pos, "empty import statement")

        for name in stmt.names:
            self.env.define_var(name, False)

        self.check_imported(stmt.path)
        """

    def check_imported(self, path: str):
        from full_passes import full_passes

        full_passes(self.error_listener, path + ".loop", self.file.path)

    def visit_VarDecl(self, stmt: VarDecl):
        if stmt.expr:
            self.check(stmt.expr)

        self.env.define_var(stmt.name, stmt.export)

    def visit_VarExpr(self, expr: VarExpr):
        # TODO: Removed dunder check because lowered before semantic check.

        """
        if expr.name.text.startswith("__"):
            self.error_listener.error(
                expr.pos,
                "double underscore at the begginning is not allowed in variable names",
            )
        """

        self.env.resolve(expr.name)

    def visit_FuncDecl(self, stmt: FuncDecl):
        self.visit_FuncProto(stmt, stmt.export)

    def visit_FuncProto(self, stmt: FuncDecl, export: bool):
        self.env.define_var(stmt.name, export)

        self.new_env(stmt.name.text)

        for arg in stmt.args:
            self.env.define_var(arg, False)

        self.check(stmt.body)

        if self.env.upvalues:
            stmt.upvalues = self.env.upvalues

        self.end_env()

    def visit_ReturnStmt(self, stmt: ReturnStmt):
        if not self.env.parent:
            self.error_listener.error(stmt.pos, "unexpected return statement")

        if self.classes and self.env.name == "init":
            if stmt.expr != None or not (
                type(stmt.expr) == VarExpr and stmt.expr.name.text == "init"
            ):
                self.error_listener.error(
                    stmt.pos, "malformed return statement in the init method"
                )

        self.check(stmt.expr)

    def visit_ClassDecl(self, stmt: ClassDecl):
        self.env.define_var(stmt.name, stmt.export)

        self.new_class(stmt.name)

        for method in stmt.methods:
            self.check(method)

        self.end_class()

    def visit_Method(self, stmt: Method):
        self.visit_FuncProto(stmt, False)

    def visit_BlockStmt(self, stmt: BlockStmt):
        self.env.new_block()

        for block_stmt in stmt.stmts:
            self.check(block_stmt)

        stmt.locals = self.env.end_block()

    def visit_Assignment(self, expr: Assignment):
        self.check(expr.var)
        self.check(expr.expr)

        if not check_assignment_target(expr.var):
            self.error_listener.error(
                expr.pos,
                "invalid assignment target",
            )

    # Utility.

    def new_env(self, name: str):
        self.env = Env(name, self.error_listener, self.env)

    def end_env(self):
        assert self.env  # Just in case.
        assert self.env.parent
        self.env = self.env.parent

    def new_class(self, name: str):
        self.classes.append(ClassDef(name))

    def end_class(self):
        assert self.classes
        self.classes.pop()

    # Trivial.

    def visit_Module(self, module: Module):
        for stmt in module.stmts:
            self.check(stmt)

        module.globals_count = len(self.env.globals)

    def visit_PrintStmt(self, stmt: PrintStmt):
        self.check(stmt.expr)

    def visit_IfStmt(self, stmt: IfStmt):
        self.check(stmt.condition)
        self.check(stmt.then_arm)
        if stmt.else_arm:
            self.check(stmt.else_arm)

    def visit_WhileStmt(self, stmt: WhileStmt):
        self.check(stmt.condition)
        self.check(stmt.block)

    def visit_ExprStmt(self, stmt: ExprStmt):
        self.check(stmt.expr)

    def visit_BinaryOp(self, expr: BinaryOp):
        self.check(expr.left)
        self.check(expr.right)

    def visit_UnaryOp(self, expr: UnaryOp):
        self.check(expr.expr)

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


def check_assignment_target(expr: Expr) -> bool:
    match expr:
        case VarExpr() | GetAttrExpr() | GetItemExpr():
            return True
