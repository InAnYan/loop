from dataclasses import dataclass
from typing import List, Optional

from util.emmiter import Emitter
from util.error_listener import ErrorListener

from loop_ast.repr import *
from loop_ast.base import *
from loop_ast.expr import *
from loop_ast.stmt import *
from loop_ast.module import *


def compile_module(
    error_listener: ErrorListener, module: Module
) -> Optional[ModuleValue]:
    return ModuleCompiler(error_listener, module.path).do(module)


class BaseCompiler(AstVisitor):
    error_listener: ErrorListener
    path: str
    emitter: Emitter

    def __init__(self, error_listener: ErrorListener, path: str) -> None:
        self.error_listener = error_listener
        self.path = path
        self.emitter = Emitter(error_listener)

    def compile(self, node: AstNode):
        return self.visit(node)

    def visit_ListLiteral(self, expr: ListLiteral):
        for item in expr.elements:
            self.compile(item)

        self.emitter.opcode(Opcode.BuildList, expr.pos)
        self.emitter.byte(len(expr.elements), expr.pos)

    def visit_Module(self, module: Module):
        for stmt in module.stmts:
            self.compile(stmt)

        last_pos = make_last_pos(module.stmts, SourcePosition(module.path, 1))
        self.emitter.opcode(Opcode.ModuleEnd, last_pos)

    def visit_ImportAsStmt(self, stmt: ImportAsStmt):
        self.emitter.add_and_process_constant(
            StringValue(stmt.path), stmt.pos, LongInst.Import
        )
        self.define_var(stmt.name)

    def visit_ImportFromStmt(self, stmt: ImportFromStmt):
        raise Exception(
            "compiler does not implement ImportFromStmt, it should be lowered"
        )

    def visit_PrintStmt(self, stmt: PrintStmt):
        self.compile(stmt.expr)
        self.emitter.opcode(Opcode.Print, stmt.pos)

    def visit_ExprStmt(self, stmt: ExprStmt):
        self.compile(stmt.expr)
        self.emitter.opcode(Opcode.Pop, stmt.pos)

    def visit_VarDecl(self, stmt: VarDecl):
        if stmt.expr:
            self.compile(stmt.expr)
        else:
            self.emitter.opcode(Opcode.PushNull, stmt.pos)

        self.define_var(stmt.name)

    def visit_LetDecl(self, stmt: LetDecl):
        raise Exception("let should be lowered")

    def visit_ReturnStmt(self, stmt: ReturnStmt):
        if stmt.expr:
            self.compile(stmt.expr)
        else:
            self.emitter.opcode(Opcode.PushNull, stmt.pos)

        self.emitter.opcode(Opcode.Return, stmt.pos)

    def visit_BlockStmt(self, stmt: BlockStmt):
        for child in stmt.stmts:
            self.compile(child)

        for local in stmt.locals:
            self.emitter.opcode(Opcode.CloseUpvalue if local else Opcode.Pop, stmt.pos)

    def visit_IfStmt(self, stmt: IfStmt):
        self.compile(stmt.condition)
        false_jump = self.emitter.jump(Opcode.JumpIfFalsePop, stmt.pos)

        self.compile(stmt.then_arm)
        then_jump = self.emitter.jump(Opcode.Jump, stmt.pos)

        self.emitter.patch_jump(false_jump, stmt.pos)
        if stmt.else_arm:
            self.compile(stmt.else_arm)

        self.emitter.patch_jump(then_jump, stmt.pos)

    def visit_WhileStmt(self, stmt: WhileStmt):
        condition = self.emitter.get_pos()

        self.compile(stmt.condition)
        exit_jump = self.emitter.jump(Opcode.JumpIfFalsePop, stmt.pos)

        self.compile(stmt.block)

        self.emitter.loop(condition, stmt.pos)

        self.emitter.patch_jump(exit_jump, stmt.pos)

    def visit_FuncDecl(self, stmt: FuncDecl):
        comp = FunctionCompiler(self.error_listener, self.path)
        func_val = comp.do(stmt)
        self.emitter.add_and_process_constant(func_val, stmt.pos, LongInst.PushConstant)

        if stmt.upvalues:
            self.emitter.opcode(Opcode.BuildClosure, stmt.pos)
            self.emitter.byte(len(stmt.upvalues), stmt.pos)

            for upvalue in stmt.upvalues:
                self.emitter.byte(1 if upvalue.is_local else 0, stmt.pos)
                self.emitter.byte(upvalue.index, stmt.pos)

        self.define_var(stmt.name)

    def visit_ClassDecl(self, stmt: ClassDecl):
        self.emitter.add_and_process_constant(
            ClassValue(
                stmt.name.text,
                [
                    FunctionCompiler(self.error_listener, self.path).do(method)
                    for method in stmt.methods
                ],
            ),
            stmt.pos,
            LongInst.PushConstant,
        )
        self.define_var(stmt.name)

    def visit_IntegerLiteral(self, expr: IntegerLiteral):
        self.emitter.add_and_process_constant(
            IntegerValue(expr.num), expr.pos, LongInst.PushConstant
        )

    def visit_BoolLiteral(self, expr: BoolLiteral):
        if expr.b:
            opcode = Opcode.PushTrue
        else:
            opcode = Opcode.PushFalse

        self.emitter.opcode(opcode, expr.pos)

    def visit_StringLiteral(self, expr: StringLiteral):
        self.emitter.add_and_process_constant(
            StringValue(expr.text), expr.pos, LongInst.PushConstant
        )

    def visit_NullLiteral(self, expr: NullLiteral):
        self.emitter.opcode(Opcode.PushNull, expr.pos)

    def visit_DictionaryLiteral(self, expr: DictionaryLiteral):
        for pair in expr.entries:
            self.compile(pair.key)
            self.compile(pair.value)

        self.emitter.opcode(Opcode.BuildDictionary, expr.pos)
        self.emitter.byte(len(expr.entries), expr.pos)

    def visit_VarExpr(self, expr: VarExpr):
        self.get_var(expr.name)

    def visit_CallExpr(self, expr: CallExpr):
        self.compile(expr.callee)

        for arg in expr.args:
            self.compile(arg)

        self.emitter.opcode(Opcode.Call, expr.pos)
        self.emitter.byte(len(expr.args), expr.pos)

    def visit_GetItemExpr(self, expr: GetItemExpr):
        self.compile(expr.obj)
        for index in expr.index:
            self.compile(index)

        self.emitter.opcode(Opcode.GetItem, expr.pos)
        self.emitter.byte(len(expr.index), expr.pos)

    def visit_Assignment(self, expr: Assignment):
        match expr.var:
            case VarExpr(_, name):
                self.compile(expr.expr)
                self.set_var(name)

            case GetItemExpr(pos, obj, indexes):
                self.compile(obj)

                for index in indexes:
                    self.compile(index)

                self.compile(expr.expr)

                self.emitter.opcode(Opcode.SetItem, pos)
                self.emitter.byte(len(indexes) + 1, pos)

            case GetAttrExpr(pos, obj, attr):
                self.compile(obj)
                self.compile(expr.expr)
                self.emitter.add_and_process_constant(
                    StringValue(attr.text), pos, LongInst.SetAttribute
                )

            case _:
                raise NotImplementedError()

    def visit_GetAttrExpr(self, expr: GetAttrExpr):
        self.compile(expr.obj)
        self.emitter.add_and_process_constant(
            StringValue(expr.attr.text), expr.pos, LongInst.GetAttribute
        )

    def visit_UnaryOp(self, expr: UnaryOp):
        self.compile(expr.expr)
        self.emitter.opcode(expr.op.to_opcode(), expr.pos)

    def visit_BinaryOp(self, expr: BinaryOp):
        match expr.op:
            case BinaryOpType.LogicalOr:
                self.compile_logical_or(expr.pos, expr.left, expr.right)
            case BinaryOpType.LogicalAnd:
                self.compile_logical_and(expr.pos, expr.left, expr.right)
            case _:
                self.compile_basic_binary(expr.pos, expr.op, expr.left, expr.right)

    def compile_logical_or(self, pos: SourcePosition, left: Expr, right: Expr):
        self.compile(left)
        true_jump = self.emitter.jump(Opcode.JumpIfTrue, pos)
        self.emitter.opcode(Opcode.Pop, pos)
        self.compile(right)
        self.emitter.patch_jump(true_jump, pos)

    def compile_logical_and(self, pos: SourcePosition, left: Expr, right: Expr):
        self.compile(left)
        false_jump = self.emitter.jump(Opcode.JumpIfFalse, pos)
        self.emitter.opcode(Opcode.Pop, pos)
        self.compile(right)
        self.emitter.patch_jump(false_jump, pos)

    def compile_basic_binary(
        self, pos: SourcePosition, op: BinaryOpType, left: Expr, right: Expr
    ):
        self.compile(left)
        self.compile(right)
        self.emitter.opcode(op.to_opcode(), pos)

        match op:
            case BinaryOpType.GreaterEqual | BinaryOpType.LessEqual | BinaryOpType.NotEqual:
                self.emitter.opcode(Opcode.Not, pos)

    def define_var(self, name: Identifier):
        assert name.ref_type

        match name.ref_type:
            case RefType.GLOBAL:
                self.emitter.opcode(Opcode.SetGlobal, name.pos)
                self.emitter.byte(name.ref_index, name.pos)
                self.emitter.opcode(Opcode.Pop, name.pos)
            case RefType.EXPORT:
                self.emitter.add_and_process_constant(
                    StringValue(name.text), name.pos, LongInst.Export
                )
            case RefType.LOCAL:
                pass
            case _:
                raise NotImplementedError()

    def get_var(self, name: Identifier):
        match name.ref_type:
            case RefType.GLOBAL | RefType.LOCAL:
                opcode = (
                    Opcode.GetGlobal
                    if name.ref_type == RefType.GLOBAL
                    else Opcode.GetLocal
                )
                self.emitter.opcode(opcode, name.pos)
                self.emitter.byte(name.ref_index, name.pos)

            case RefType.EXPORT:
                self.emitter.add_and_process_constant(
                    StringValue(name.text), name.pos, LongInst.GetExport
                )

            case RefType.UPVALUE:
                self.emitter.opcode(Opcode.GetUpvalue, name.pos)
                self.emitter.byte(name.ref_index, name.pos)

            case x:
                raise NotImplementedError(x)

    def set_var(self, name: Identifier):
        match name.ref_type:
            case RefType.GLOBAL | RefType.LOCAL:
                opcode = (
                    Opcode.SetGlobal
                    if name.ref_type == RefType.GLOBAL
                    else Opcode.SetLocal
                )
                self.emitter.opcode(opcode, name.pos)
                self.emitter.byte(name.ref_index, name.pos)

            case RefType.EXPORT:
                self.emitter.add_and_process_constant(
                    StringValue(name.text), name.pos, LongInst.SetExport
                )

            case RefType.UPVALUE:
                self.emitter.opcode(Opcode.SetUpvalue, name.pos)
                self.emitter.byte(name.ref_index, name.pos)

            case x:
                raise NotImplementedError(x)


class FunctionCompiler(BaseCompiler):
    def __init__(self, error_listener: ErrorListener, path: str):
        super().__init__(error_listener, path)

    def do(self, func: FuncDecl) -> FunctionValue:
        self.compile(func.body)

        last_pos = make_last_pos(func.body.stmts, func.pos)

        if len(self.emitter.code) != 0 and self.emitter.code[-1] != Opcode.Return.value:
            if type(func) == Method and func.name.text == "init":
                self.emitter.opcode(Opcode.GetLocal, last_pos)
                self.emitter.byte(0, last_pos)
                self.emitter.opcode(Opcode.Return, last_pos)
            else:
                self.emitter.opcode(Opcode.PushNull, last_pos)
                self.emitter.opcode(Opcode.Return, last_pos)

        return FunctionValue(
            func.name.text,
            len(func.args),
            self.emitter.make_chunk(),
        )


class ModuleCompiler(BaseCompiler):
    def __init__(self, error_listener: ErrorListener, path: str):
        super().__init__(error_listener, path)

    def do(self, module: Module) -> Optional[ModuleValue]:
        self.compile(module)

        if not self.error_listener.had_error:
            return ModuleValue(module.globals_count, self.emitter.make_chunk())


def make_last_pos(lst: List[AstNode], default: SourcePosition) -> SourcePosition:
    if len(lst) == 0:
        return default
    else:
        return lst[-1].pos
