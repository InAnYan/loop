from __future__ import annotations
from typing import Callable, List, Optional
import os
from lark.lark import Lark
from lark import UnexpectedToken, v_args
from lark.visitors import Transformer
from lark.lexer import Token
from lark.tree import Meta

from loop_ast import (
    Assignment,
    BinaryOp,
    BinaryOpType,
    BlockStmt,
    BoolLiteral,
    Expr,
    ExprStmt,
    Identifier,
    IfStmt,
    IntegerLiteral,
    Module,
    NullLiteral,
    PrintStmt,
    SourcePosition,
    Stmt,
    StringLiteral,
    UnaryOp,
    UnaryOpType,
    VarDecl,
    VarExpr,
)
from error_listener import ErrorListener


dirname = os.path.dirname(__file__)
GRAMMAR_PATH = os.path.join(dirname, "grammar.lark")


def parse_loop_module(
    error_listener: ErrorListener, path: str, source: str
) -> Optional[Module]:
    had_error = False

    def handle_error(error: UnexpectedToken) -> bool:
        nonlocal had_error, error_listener, path

        had_error = True

        error_listener.error(
            SourcePosition(path, error.line), f"unexpected token {error.token}"
        )

        # TODO: Add synchronization.

        return True

    with open(GRAMMAR_PATH) as grammar_file:
        grammar = grammar_file.read()
        lark = Lark(grammar, parser="lalr", start="module", propagate_positions=True)
        tree = lark.parse(source, on_error=handle_error)

        if had_error:
            return None

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


def token_fn(kind, process):
    def fn(self: LarkTreeToLoopAst, token: Token) -> kind:
        return kind(self.make_pos(token), process(token))

    return fn


def token_to_bool(token: Token) -> bool:
    return str(token) == "true"


def make_str(token: Token) -> str:
    return str(token)[1:-1]


def tree(kind):
    def fn(self: LarkTreeToLoopAst, meta: Meta, *args) -> kind:
        return kind(self.make_pos(meta), *args)

    return fn


def return_none(x):
    return None


@v_args(inline=True, meta=True)
class LarkTreeToLoopAst(Transformer):
    path: str

    def __init__(self, path: str) -> None:
        self.path = path

    # Statements.

    def module(self, _meta: Meta, *stmts: List[Stmt]) -> Module:
        return Module(self.path, stmts)

    def block_stmt(self, meta: Meta, *stmts: List[Stmt]) -> BlockStmt:
        return BlockStmt(self.make_pos(meta), stmts)

    print_stmt = tree(PrintStmt)
    expr_stmt = tree(ExprStmt)
    var_decl = tree(VarDecl)
    if_stmt = tree(IfStmt)

    # Expressions.

    assignment = tree(Assignment)

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

    var_expr = tree(VarExpr)

    # Tokens.

    INT = token_fn(IntegerLiteral, int)

    TRUE = token_fn(BoolLiteral, token_to_bool)
    FALSE = token_fn(BoolLiteral, token_to_bool)

    IDENTIFIER = token_fn(Identifier, str)
    STRING = token_fn(StringLiteral, make_str)

    NULL = token_fn(NullLiteral, return_none)

    # Private.

    def make_pos(self, token: Token | Meta) -> SourcePosition:
        return SourcePosition(self.path, token.line)
