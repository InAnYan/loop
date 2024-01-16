from __future__ import annotations
from typing import Callable, List

from lark.lark import Lark
from lark import v_args
from lark.visitors import Transformer
from lark.lexer import Token
from lark.tree import Meta

from loop_ast import (
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
    UnaryOpType,
)


GRAMMAR_PATH = "grammar.lark"


def parse_loop_module(path: str, source: str) -> Module:
    with open(GRAMMAR_PATH) as grammar_file:
        grammar = grammar_file.read()
        lark = Lark(grammar, parser="lalr", start="module", propagate_positions=True)
        tree = lark.parse(source)
        transformer = LarkTreeToLoopAst(path)
        return transformer.transform(tree)


def unary_op(op: UnaryOpType) -> Callable[[Expr], UnaryOp]:
    def fn(self: LarkTreeToLoopAst, meta: Meta, expr: Expr) -> UnaryOp:
        return UnaryOp(self.make_pos(meta), op, expr)

    return fn


def bin_op(op: BinaryOpType) -> Callable[[Expr, Expr], BinaryOp]:
    def fn(self: LarkTreeToLoopAst, meta: Meta, left: Expr, right: Expr) -> BinaryOp:
        return BinaryOp(self.make_pos(meta), op, left, right)

    return fn


def bool_lit(b: bool) -> Callable[[Token], BoolLiteral]:
    def fn(self: LarkTreeToLoopAst, token: Token) -> BoolLiteral:
        return BoolLiteral(self.make_pos(token), b)

    return fn


@v_args(inline=True, meta=True)
class LarkTreeToLoopAst(Transformer):
    path: str

    def __init__(self, path: str) -> None:
        self.path = path

    # Statements.

    def module(self, _meta: Meta, *stmts: List[Stmt]) -> Module:
        return Module(self.path, stmts)

    def print_stmt(self, meta: Meta, expr: Expr) -> PrintStmt:
        return PrintStmt(self.make_pos(meta), expr)

    def expr_stmt(self, meta: Meta, expr: Expr) -> PrintStmt:
        return ExprStmt(self.make_pos(meta), expr)

    # Expressions.

    logical_or = bin_op(BinaryOpType.LogicalOr)
    logical_and = bin_op(BinaryOpType.LogicalAnd)

    eq = bin_op(BinaryOpType.Equal)
    neq = bin_op(BinaryOpType.NotEqual)

    gt = bin_op(BinaryOpType.Greater)
    lt = bin_op(BinaryOpType.Less)
    gte = bin_op(BinaryOpType.GreaterEqual)
    lte = bin_op(BinaryOpType.LessEqual)

    add = bin_op(BinaryOpType.Add)
    sub = bin_op(BinaryOpType.Subtract)
    mul = bin_op(BinaryOpType.Multiply)
    div = bin_op(BinaryOpType.Divide)

    add_unary = unary_op(UnaryOpType.Plus)
    neg = unary_op(UnaryOpType.Negate)
    logical_not = unary_op(UnaryOpType.Not)

    # Tokens.

    def INT(self, token: Token):
        return IntegerLiteral(self.make_pos(token), int(token))

    TRUE = bool_lit(True)
    FALSE = bool_lit(False)

    # Private.

    def make_pos(self, token: Token | Meta) -> SourcePosition:
        return SourcePosition(self.path, token.line)
