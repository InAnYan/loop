from typing import List, Optional
from emmiter import Emitter
from error_listener import ErrorListener

from repr import NumberValue, Opcode, Value
from loop_ast import (
    AstNode,
    BinaryOp,
    BinaryOpType,
    BoolLiteral,
    Expr,
    ExprStmt,
    IntegerLiteral,
    Module,
    PrintStmt,
    SourcePosition,
    Stmt,
    UnaryOp,
)


class ModuleCompiler:
    error_listener: ErrorListener
    module: Module
    emitter: Emitter

    def __init__(self, error_listener: ErrorListener, module: Module) -> None:
        self.error_listener = error_listener
        self.module = module
        self.emitter = Emitter(error_listener)

    def compile_module(self) -> Optional[dict]:
        for stmt in self.module.stmts:
            self.compile(stmt)

        self.emitter.opcode(
            Opcode.Return,
            self.module.stmts[-1].pos
            if len(self.module.stmts) != 0
            else SourcePosition(self.module.path, 1),
        )

        if not self.error_listener.had_error:
            return self.emitter.make_json_chunk_object()

    def compile(self, ast: Expr | Stmt):
        if isinstance(ast, Expr):
            self.compile_expr(ast)
        elif isinstance(ast, Stmt):
            self.compile_stmt(ast)
        else:
            raise Exception(f"wrong AST hierarchy for {type(ast)}")

    def compile_stmt(self, stmt: Stmt):
        match stmt:
            case PrintStmt(pos, expr):
                self.compile(expr)
                self.emitter.opcode(Opcode.Print, pos)

            case ExprStmt(pos, expr):
                self.compile(expr)
                self.emitter.opcode(Opcode.Pop, pos)

            case _:
                raise Exception(f"unhandled compile_stmt for {type(stmt)}")

    def compile_expr(self, expr: Expr):
        match expr:
            case IntegerLiteral(pos, num):
                index = self.emitter.constant(NumberValue(num), pos)
                self.emitter.push_constant(index, pos)

            case BoolLiteral(pos, b):
                if b:
                    opcode = Opcode.PushTrue
                else:
                    opcode = Opcode.PushFalse

                self.emitter.opcode(opcode, pos)

            case UnaryOp(pos, op, expr):
                self.compile(expr)
                self.emitter.opcode(op.to_opcode(), pos)

            case BinaryOp(pos, op, left, right):
                match op:
                    case BinaryOpType.LogicalOr:
                        self.compile_logical_or(pos, left, right)
                    case BinaryOpType.LogicalAnd:
                        self.compile_logical_and(pos, left, right)
                    case _:
                        self.compile_basic_binary(pos, op, left, right)

            case _:
                raise Exception(f"unhandled compile_expr for {type(expr)}")

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
