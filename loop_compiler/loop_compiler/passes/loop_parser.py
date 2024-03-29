from __future__ import annotations
from typing import Any, Callable, List, Optional, Tuple
import os
from lark.lark import Lark
from lark import (
    UnexpectedCharacters,
    UnexpectedEOF,
    UnexpectedInput,
    UnexpectedToken,
    v_args,
)
from lark.visitors import Transformer
from lark.lexer import Token
from lark.tree import Meta

from loop_compiler.loop_ast.base import *
from loop_compiler.loop_ast.expr import *
from loop_compiler.loop_ast.loop_patterns import IdentifierPattern, ListPattern
from loop_compiler.loop_ast.stmt import *
from loop_compiler.loop_ast.module import *

from loop_compiler.util.error_listener import ErrorListener


dirname = os.path.dirname(__file__)
GRAMMAR_PATH = os.path.join(dirname, "grammar.lark")


def parse_loop_module(error_listener: ErrorListener, file: File) -> Optional[Module]:
    had_error = False

    def handle_error(error: UnexpectedInput) -> bool:
        nonlocal had_error, error_listener, file

        had_error = True
        # TODO: end_range is not correct.
        end_range = 1
        match error:
            case UnexpectedToken():
                err_str = f"unexpected token {error.token}"
                end_range = len(error.token)
            case UnexpectedEOF():
                err_str = "unexpected end of file"
            case UnexpectedCharacters():
                err_str = "unexpected characters"

        error_listener.error(
            SourcePosition(
                file,
                SourcePoint(error.line, error.column),
                SourcePoint(error.line, error.column + end_range),
            ),
            err_str,
        )

        # TODO: Add synchronization.

        return True

    with open(GRAMMAR_PATH) as grammar_file:
        grammar = grammar_file.read()
        lark = Lark(grammar, parser="lalr", start="module", propagate_positions=True)

        # TODO: Lark error handling not working.
        try:
            tree = lark.parse(file.contents, on_error=handle_error)
        except UnexpectedEOF as e:
            handle_error(e)
        except UnexpectedToken as e:
            handle_error(e)
        except UnexpectedCharacters as e:
            handle_error(e)

        if had_error:
            return None

        transformer = LarkTreeToLoopAst(file)
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


def some_list(t):
    def fn(self: LarkTreeToLoopAst, meta: Meta, *args: Tuple[t, ...]) -> List[t]:
        lst = list(args)
        if lst == [None]:
            lst = []
        return lst

    return fn


@v_args(inline=True, meta=True)
class LarkTreeToLoopAst(Transformer):
    file: File

    def __init__(self, file: File) -> None:
        self.file = file

    # Statements.

    def module(self, _meta: Meta, *stmts: Stmt) -> Module:
        return Module(self.file.path, list(stmts))

    def import_as(
        self, meta: Meta, path: StringLiteral, as_name: Identifier
    ) -> ImportAsStmt:
        return ImportAsStmt(self.make_pos(meta), path.text, as_name)

    def import_from(
        self, meta: Meta, names: List[Identifier], path: StringLiteral
    ) -> ImportFromStmt:
        return ImportFromStmt(self.make_pos(meta), names, path.text)

    def block_stmt(self, meta: Meta, *stmts: Stmt) -> BlockStmt:
        return BlockStmt(self.make_pos(meta), list(stmts))

    print_stmt = tree(PrintStmt)
    expr_stmt = tree(ExprStmt)
    var_decl = tree(VarDecl)
    let_decl = tree(LetDecl)
    if_stmt = tree(IfStmt)
    while_stmt = tree(WhileStmt)
    func_decl = tree(FuncDecl)
    return_stmt = tree(ReturnStmt)

    arg_list = some_list(Identifier)

    class_decl = tree(ClassDecl)
    method = tree(Method)

    def method_list(self, meta: Meta, *method: Method) -> List[Method]:
        return list(method)

    def maybe_inherit(
        self, meta: Meta, name: Optional[Identifier]
    ) -> Optional[Identifier]:
        return name

    try_stmt = tree(TryStmt)
    throw_stmt = tree(ThrowStmt)

    for_in_stmt = tree(ForInStmt)
    break_stmt = tree(BreakStmt)
    continue_stmt = tree(ContinueStmt)

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

    is_expr = bin_op(BinaryOpType.Is)

    add_unary = unary_op(UnaryOpType.Plus)
    neg = unary_op(UnaryOpType.Negate)
    logical_not = unary_op(UnaryOpType.Not)

    var_expr = tree(VarExpr)
    call = tree(CallExpr)
    getattr = tree(GetAttrExpr)
    getitem = tree(GetItemExpr)

    dictionary = tree(DictionaryLiteral)
    dict_pair = tree(DictionaryPair)

    list_lit = tree(ListLiteral)

    expr_list = some_list(Expr)
    dict_list = some_list(DictionaryPair)

    # Patterns.

    ident_pat = tree(IdentifierPattern)

    def list_pat(self, meta: Meta, *patterns: Pattern) -> ListPattern:
        return ListPattern(self.make_pos(meta), list(patterns))

    # Tokens.

    INT = token_fn(IntegerLiteral, int)

    TRUE = token_fn(BoolLiteral, token_to_bool)
    FALSE = token_fn(BoolLiteral, token_to_bool)

    IDENTIFIER = token_fn(Identifier, str)
    STRING = token_fn(StringLiteral, make_str)

    NULL = token_fn(NullLiteral, return_none)

    def export(self, meta: Meta, res: bool = False) -> bool:
        return res

    def EXPORT(self, token: Token) -> bool:
        return not token == ""

    # Private.

    def make_pos(self, token: Token | Meta) -> SourcePosition:
        assert token.line
        assert token.column
        assert token.end_line
        assert token.end_column

        return SourcePosition(
            self.file,
            SourcePoint(token.line, token.column),
            SourcePoint(token.end_line, token.end_column),
        )
