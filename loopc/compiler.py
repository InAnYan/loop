from dataclasses import dataclass
from typing import List, Optional
from emmiter import Emitter
from error_listener import ErrorListener

from repr import LongInst, NumberValue, Opcode, StringValue, Value
from loop_ast import (
    Assignment,
    AstNode,
    AstVisitor,
    BinaryOp,
    BinaryOpType,
    BlockStmt,
    BoolLiteral,
    Expr,
    ExprStmt,
    Identifier,
    IntegerLiteral,
    Module,
    NullLiteral,
    PrintStmt,
    SourcePosition,
    Stmt,
    StringLiteral,
    UnaryOp,
    VarDecl,
    VarExpr,
)


@dataclass
class Local:
    ident: Identifier
    scope: int


class ModuleCompiler(AstVisitor):
    error_listener: ErrorListener
    emitter: Emitter
    scope: int
    locals: List[Local]

    def __init__(self, error_listener: ErrorListener) -> None:
        self.error_listener = error_listener
        self.emitter = Emitter(error_listener)
        self.scope = 0
        self.locals = []

    def compile(self, node: AstNode):
        return self.visit(node)

    def visit_Module(self, module: Module) -> Optional[dict]:
        for stmt in module.stmts:
            self.compile(stmt)

        if len(module.stmts) != 0:
            last_pos = module.stmts[-1].pos
        else:
            last_pos = SourcePosition(module.path, 1)

        self.emitter.opcode(Opcode.PushNull, last_pos)
        self.emitter.opcode(Opcode.Return, last_pos)

        if not self.error_listener.had_error:
            return self.emitter.make_json_chunk_object()

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

    def visit_BlockStmt(self, stmt: BlockStmt):
        self.begin_scope()

        for child in stmt.stmts:
            self.compile(child)

        self.end_scope(stmt.pos)  # TODO: Wrong pos for BlockStmt compile.

    def visit_IntegerLiteral(self, expr: IntegerLiteral):
        self.emitter.add_and_process_constant(
            NumberValue(expr.num), expr.pos, LongInst.PushConstant
        )

    def visit_BoolLiteral(self, expr: BoolLiteral):
        if expr.b:
            opcode = Opcode.PushTrue
        else:
            opcode = Opcode.PushFalse

        self.emitter.opcode(opcode, expr.pos)

    def visit_StringLiteral(self, expr: StringLiteral):
        self.emitter.add_and_process_constant(
            StringValue(expr.txt), expr.pos, LongInst.PushConstant
        )

    def visit_NullLiteral(self, expr: NullLiteral):
        self.emitter.opcode(Opcode.PushNull, expr.pos)

    def visit_VarExpr(self, expr: VarExpr):
        for i, local in enumerate(reversed(self.locals)):
            if local.ident.text == expr.name.text:
                self.emitter.opcode(
                    Opcode.GetLocal, expr.pos
                )  # TODO: Should GetLocal be a LongInst?
                self.emitter.byte(len(self.locals) - i - 1, expr.pos)
                return

        self.emitter.add_and_process_constant(
            StringValue(expr.name.text), expr.pos, LongInst.GetGlobal
        )

    def visit_Assignment(self, expr: Assignment):
        if not isinstance(expr.var, VarExpr):
            self.error_listener.error(
                expr.pos,
                "invalid assignment target (expected variable)",
            )
        else:
            self.compile(expr.expr)
            self.set_var(expr.var.name)

    def visit_UnaryOp(self, expr: UnaryOp):
        self.compile(expr)
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
        if self.scope == 0:
            self.emitter.add_and_process_constant(
                StringValue(name.text), name.pos, LongInst.DefineGlobal
            )
        else:
            self.new_local(name)

    def set_var(self, name: Identifier):
        for i, local in enumerate(reversed(self.locals)):
            if local.ident.text == name.text:
                self.emitter.opcode(
                    Opcode.SetLocal, name.pos
                )  # TODO: Should SetLocal be a LongInst?
                self.emitter.byte(len(self.locals) - i - 1, name.pos)
                return

        self.emitter.add_and_process_constant(
            StringValue(name.str), name.pos, LongInst.GetGlobal
        )

    def new_local(self, name: Identifier):
        # There is semantic check, so no need to check here.
        self.locals.append(Local(name, self.scope))

    def begin_scope(self):
        self.scope += 1

    def end_scope(self, pos: SourcePosition):
        assert self.scope != 0

        while len(self.locals) != 0 and self.locals[-1].scope == self.scope:
            self.emitter.opcode(Opcode.Pop, pos)
            self.locals.pop()

        self.scope -= 1
