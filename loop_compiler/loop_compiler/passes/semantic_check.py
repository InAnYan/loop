from __future__ import annotations
from dataclasses import dataclass

from typing import List, Optional
from loop_compiler.loop_ast.expr import (
    Assignment,
    BinaryOp,
    BoolLiteral,
    CallExpr,
    DictionaryLiteral,
    GetAttrExpr,
    GetItemExpr,
    IntegerLiteral,
    ListLiteral,
    NullLiteral,
    StringLiteral,
    UnaryOp,
    VarExpr,
)
from loop_compiler.loop_ast.module import Module
from loop_compiler.loop_ast.loop_patterns import IdentifierPattern, ListPattern, Pattern
from loop_compiler.loop_ast.stmt import (
    BlockStmt,
    BreakStmt,
    ClassDecl,
    ContinueStmt,
    ExprStmt,
    ForInStmt,
    FuncDecl,
    IfStmt,
    ImportAsStmt,
    ImportFromStmt,
    LetDecl,
    Method,
    PrintStmt,
    ReturnStmt,
    ThrowStmt,
    TryStmt,
    Upvalue,
    VarDecl,
    WhileStmt,
)
from loop_compiler.passes.read_file import read_loop_file

from loop_compiler.util.error_listener import ErrorListener
from loop_compiler.loop_ast.base import (
    AstNode,
    AstVisitor,
    File,
    Identifier,
    RefType,
    SourcePoint,
    SourcePosition,
)


def perform_semantic_check(
    file: File, error_listener: ErrorListener, module: Module, **kwargs
) -> bool:
    SemanticCheck(file, error_listener, **kwargs).check(module)
    return not error_listener.had_error


@dataclass
class Local:
    name: Identifier
    scope: int
    is_captured: bool
    is_final: bool


@dataclass
class ClassDef:
    name: Identifier


THIS_IDENT = Identifier(
    SourcePosition("", SourcePoint(0, 0), SourcePoint(0, 0)), "this", RefType.LOCAL, 0
)


class Env:
    name: str
    error_listener: ErrorListener
    parent: Optional[Env]
    defs: List[Local]
    globals: List[Local]  # A dirty hack, but easy to use
    exports: List[Local]
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
            Local(
                (
                    Identifier(
                        SourcePosition("", SourcePoint(0, 0), SourcePoint(0, 0)), ""
                    )
                    if self.scope == 0
                    else THIS_IDENT
                ),
                self.scope,
                False,
                True,
            )
        ]
        self.globals = []
        self.exports = []
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

    def define_var(self, name: Identifier, export: bool, is_final: bool):
        self.check_redefinition(name)
        if export:
            self.check_export(name)

        if self.scope == 0:
            if export:
                lst = self.exports
            else:
                lst = self.globals
        else:
            lst = self.defs

        lst.append(Local(name, self.scope, False, is_final))

        name.ref_type = (
            RefType.LOCAL
            if self.scope > 0
            else RefType.EXPORT if export else RefType.GLOBAL
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

        if self.resolve_export(name):
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

    def resolve_export(self, name: Identifier) -> bool:
        if self.parent:
            return self.parent.resolve_export(name)
        else:
            for deff in self.exports:
                if deff.name.text == name.text:
                    name.ref_type = deff.name.ref_type
                    name.ref_index = deff.name.ref_index
                    return True

        return False

    # Very bad.
    def check_assignable(self, name: Identifier) -> bool:
        for deff in reversed(self.globals + self.exports + self.defs):
            if deff.name.text == name.text:
                return not deff.is_final

        if self.parent:
            return self.parent.check_assignable(name)

        raise Exception(f"not asssignable {name.text}, parent: {self.parent}")


@dataclass
class Loop:
    scope: int


class SemanticCheck(AstVisitor):
    file: File
    error_listener: ErrorListener
    env: Env
    classes: List[ClassDef]
    loops: List[Loop]
    compile_imported: bool

    def __init__(self, file: File, error_listener: ErrorListener, **kwargs):
        self.file = file
        self.error_listener = error_listener
        self.env = Env("<script>", error_listener)
        self.classes = []
        self.loops = []
        self.compile_imported = kwargs.get("compile_imported", False)

    def check(self, node: AstNode):
        self.visit(node)

    def visit_ListLiteral(self, expr: ListLiteral):
        for item in expr.elements:
            self.check(item)

    def visit_ImportAsStmt(self, stmt: ImportAsStmt):
        self.env.define_var(stmt.name, False, True)

        if stmt.path != "builtins":
            self.check_imported(stmt.path, stmt.pos)

    def visit_ImportFromStmt(self, stmt: ImportFromStmt):
        if len(stmt.names) == 0:
            self.error_listener.error(stmt.pos, "empty import statement")

        for name in stmt.names:
            self.env.define_var(name, False, False)

        self.check_imported(stmt.path, stmt.pos)

    def check_imported(self, path: str, pos: SourcePosition):
        from loop_compiler.full_passes import (
            full_passes,
            resolve_path,
            str_to_loop_module_checked,
        )

        if self.compile_imported:
            full_passes(self.error_listener, path + ".loop", pos)
        else:
            # TODO: Full passes has a check of times. But str to loop module not.
            if file := read_loop_file(resolve_path(path + ".loop")):
                str_to_loop_module_checked(
                    self.error_listener, file, compile_imported=self.compile_imported
                )
            else:
                self.error_listener.error(pos, f"file not found: '{path}'")

    def visit_VarDecl(self, stmt: VarDecl, **kwargs):
        collecting_globals = kwargs.get("collecting_globals", False)

        if stmt.expr and not collecting_globals:
            self.check(stmt.expr)

        if collecting_globals or self.env.scope != 0:
            self.pattern(stmt.pattern, stmt.export, False)

    def pattern(self, pattern: Pattern, export: bool, is_final: bool):
        match pattern:
            case IdentifierPattern(pos, ident):
                self.env.define_var(ident, export, is_final)
            case ListPattern(patterns):
                raise Exception("should be lowered")
            case _:
                raise NotImplementedError(f"unknown pattern: {pattern}")

    def visit_LetDecl(self, stmt: VarDecl, **kwargs):
        collecting_globals = kwargs.get("collecting_globals", False)

        if stmt.expr and not collecting_globals:
            self.check(stmt.expr)

        if collecting_globals or self.env.scope != 0:
            self.pattern(stmt.pattern, stmt.export, False)

    def visit_VarExpr(self, expr: VarExpr):
        # TODO: Removed dunder check because lowered before semantic check.

        """
        if expr.name.text.startswith("__"):
            self.error_listener.error(
                expr.pos,
                "double underscore at the begginning is not allowed in variable names",
            )
        """
        if expr.name.text == "super":
            if self.env.scope == 0:
                self.error_listener.error(expr.pos, "cannot use super there")
            else:
                pass
        else:
            self.env.resolve(expr.name)

    def visit_FuncDecl(self, stmt: FuncDecl, **kwargs):
        self.visit_FuncProto(stmt, stmt.export, **kwargs)

    def visit_FuncProto(self, stmt: FuncDecl, export: bool, **kwargs):
        collecting_globals = kwargs.get("collecting_globals", False)

        if collecting_globals or self.env.scope != 0:
            self.env.define_var(stmt.name, export, True)

        if not collecting_globals:
            self.new_env(stmt.name.text)

            for arg in stmt.args:
                self.env.define_var(arg, False, False)

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

        if stmt.expr:
            self.check(stmt.expr)

    def visit_ClassDecl(self, stmt: ClassDecl, **kwargs):
        collecting_globals = kwargs.get("collecting_globals", False)

        if collecting_globals or self.env.scope != 0:
            self.env.define_var(stmt.name, stmt.export, True)

        if not collecting_globals:
            if stmt.parent:
                self.env.resolve(stmt.parent)

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
        # TODO: Destructuring.

        match expr.var:
            case VarExpr(pos, name):
                if name.text == "super" or not self.env.check_assignable(name):
                    self.error_listener.error(
                        expr.pos, "cannot assign '" + name.text + "'"
                    )
                else:
                    self.check(expr.var)

            case GetAttrExpr() | GetItemExpr():
                self.check(expr.var)

            case _:
                self.error_listener.error(expr.pos, "invalid assignment target")

        self.check(expr.expr)

    # Utility.

    def new_env(self, name: str):
        self.env = Env(name, self.error_listener, self.env)

    def end_env(self):
        assert self.env  # Just in case.
        assert self.env.parent
        self.env = self.env.parent

    def new_class(self, name: Identifier):
        self.classes.append(ClassDef(name))
        self.env.new_block()

    def end_class(self):
        assert self.classes
        self.env.end_block()
        self.classes.pop()

    # Trivial.

    def visit_Module(self, module: Module):
        self.collect_globals(module)

        for stmt in module.stmts:
            self.check(stmt)

        module.globals_count = len(self.env.globals)

    def collect_globals(self, module: Module):
        for stmt in module.stmts:
            match stmt:
                case VarDecl() | LetDecl() | FuncDecl() | ClassDecl():
                    self.visit(stmt, collecting_globals=True)

    def visit_PrintStmt(self, stmt: PrintStmt):
        self.check(stmt.expr)

    def visit_IfStmt(self, stmt: IfStmt):
        self.check(stmt.condition)
        self.check(stmt.then_arm)
        if stmt.else_arm:
            self.check(stmt.else_arm)

    def visit_WhileStmt(self, stmt: WhileStmt):
        self.check(stmt.condition)
        self.loops.append(Loop(self.env.scope))
        self.check(stmt.block)
        self.loops.pop()

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

    def visit_ThrowStmt(self, stmt: ThrowStmt):
        self.check(stmt.expr)

    def visit_TryStmt(self, stmt: TryStmt):
        self.check(stmt.try_block)

        self.env.new_block()
        self.env.define_var(stmt.catch_name, False, True)
        self.check(stmt.catch_block)
        stmt.catch_block.locals = (
            self.env.end_block() + stmt.catch_block.locals
        )  # Ooo, be very careful.

    def visit_ForInStmt(self, stmt: ForInStmt):
        self.env.new_block()
        self.pattern(stmt.pattern, False, True)
        self.check(stmt.expr)
        self.loops.append(Loop(self.env.scope))
        self.check(stmt.body)
        self.loops.pop()
        stmt.body.locals = (
            self.env.end_block() + stmt.body.locals
        )  # Ooo, be very careful.

    def visit_BreakStmt(self, stmt: BreakStmt):
        if not self.loops:
            self.error_listener.error(stmt.pos, "cannot break outside of loop")

    def visit_ContinueStmt(self, stmt: ContinueStmt):
        if not self.loops:
            self.error_listener.error(stmt.pos, "cannot continue outside of loop")
