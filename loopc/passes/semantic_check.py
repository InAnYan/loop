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


class EnvType(Enum):
    MODULE = auto()
    FUNC = auto()
    CLASS = auto()


@dataclass
class Def:
    name: Identifier
    scope: int
    exported: bool = False
    # is upvalue or is local


THIS_IDENT = Identifier(SourcePosition('', 0), 'this', RefType.LOCAL, 0)


class Env:
    kind: EnvType
    name: str
    error_listener: ErrorListener
    parent: Optional[Env]
    defs: List[Def]
    scope: int

    def __init__(self, kind: EnvType, name: str, error_listener: ErrorListener, parent: Optional[Env] = None):
        self.kind = kind
        self.name = name
        self.error_listener = error_listener
        self.parent = parent
        self.scope = 0 if parent is None else parent.scope + 1
        self.defs = [Def(THIS_IDENT, self.scope, False)] if kind == EnvType.FUNC else []

    def new_block(self):
        self.scope += 1
        
    def end_block(self) -> int:
        assert self.scope > 0

        count = 0
        for deff in reversed(self.defs):
            if deff.scope == self.scope:
                count += 1
        
        self.scope -= 1
        return count

    def define_var(self, name: Identifier, export: bool):
        self.check_redefinition(name)
        if export:
            self.check_export(name)

        self.defs.append(Def(name, self.scope, export))

        name.ref_type = RefType.LOCAL if self.scope > 0 else RefType.EXPORT if export else RefType.GLOBAL
        name.ref_index = len(self.defs) - 1

    def check_redefinition(self, name: Identifier):
        for deff in reversed(self.defs):
            if deff.scope == self.scope and deff.name.text == name.text:
                self.error_listener.error(
                    name.pos, f"variable '{name.text}' is already defined"
                )
                self.error_listener.note(deff.name.pos, f"previous definition of '{name.text}'")
                break

    def check_export(self, name: Identifier):
        if self.scope != 0:
            self.error_listener.error(name.pos, "cannot export not a top-level definition")

    def resolve(self, name: Identifier):
        for deff in reversed(self.defs):
            if deff.name.text == name.text:
                name.ref_type = deff.name.ref_type
                name.ref_index = deff.name.ref_index
                return

        if self.parent:
            self.parent.resolve(name)
        else:
            self.error_listener.error(name.pos, f"variable '{name.text}' is not defined")

    def is_in(self, kind: EnvType) -> bool:
        return self.kind == kind or (self.parent and self.parent.is_in(kind))


class SemanticCheck(AstVisitor):
    file: File
    error_listener: ErrorListener
    env: Env

    def __init__(self, file: File, error_listener: ErrorListener):
        self.file = file
        self.error_listener = error_listener
        self.env = Env(EnvType.MODULE, '<script>', error_listener)

    def check(self, node: AstNode):
        self.visit(node)
    
    def visit_ImportAsStmt(self, stmt: ImportAsStmt):
        self.env.define_var(stmt.name, False)

        self.check_imported(stmt.path)

    def visit_ImportFromStmt(self, stmt: ImportFromStmt):
        raise Exception('should be lowered')
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
        
        self.new_env(EnvType.FUNC, stmt.name.text)

        for arg in stmt.args:
            self.env.define_var(arg, False)

        self.check(stmt.body)
        
        self.end_env()

    def visit_ReturnStmt(self, stmt: ReturnStmt):
        if self.env.kind != EnvType.FUNC:
            self.error_listener.error(stmt.pos, "unexpected return statement")

        if self.env.is_in(EnvType.CLASS) and self.env.is_in(EnvType.FUNC) and self.env.name == "init":
            if stmt.expr != None or not (
                type(stmt.expr) == VarExpr and stmt.expr.name.text == "init"
            ):
                self.error_listener.error(
                    stmt.pos, "malformed return statement in the init method"
                )

        self.check(stmt.expr)

    def visit_ClassDecl(self, stmt: ClassDecl):
        self.env.define_var(stmt.name, stmt.export)
        
        self.new_env(EnvType.CLASS, stmt.name.text)
        
        for method in stmt.methods:
            self.check(method)
            
        self.end_env()

    def visit_Method(self, stmt: Method):
        self.visit_FuncProto(stmt, False)

    def visit_BlockStmt(self, stmt: BlockStmt):
        self.env.new_block()

        for block_stmt in stmt.stmts:
            self.check(block_stmt)

        stmt.locals_count = self.env.end_block()

    def visit_Assignment(self, expr: Assignment):
        self.check(expr.var)
        self.check(expr.expr)

        if not check_assignment_target(expr.var):
            self.error_listener.error(
                expr.pos,
                "invalid assignment target",
            )

    # Utility.
    
    def new_env(self, kind: EnvType, name: str):
        self.env = Env(kind, name, self.error_listener, self.env)

    def end_env(self):
        assert self.env  # Just in case.
        assert self.env.parent
        self.env = self.env.parent

    # Trivial.

    def visit_Module(self, module: Module):
        for stmt in module.stmts:
            self.check(stmt)

        module.globals_count = len(self.env.defs)

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
