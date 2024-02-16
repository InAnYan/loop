from dataclasses import dataclass, field
from typing import List, Optional
from loop_compiler.loop_ast.base import AstNode, Identifier

from loop_compiler.loop_ast.expr import Expr
from loop_compiler.loop_ast.loop_patterns import Pattern


@dataclass
class Stmt(AstNode):
    pass


@dataclass
class ImportAsStmt(Stmt):
    path: str
    name: Identifier


@dataclass
class ImportFromStmt(Stmt):
    names: List[Identifier]
    path: str


@dataclass
class PrintStmt(Stmt):
    expr: Expr


@dataclass
class ExprStmt(Stmt):
    expr: Expr


@dataclass
class VarDecl(Stmt):
    export: bool
    pattern: Pattern
    expr: Optional[Expr] = None


@dataclass
class LetDecl(Stmt):
    export: bool
    pattern: Pattern
    expr: Optional[Expr] = None


@dataclass
class BlockStmt(Stmt):
    stmts: List[Stmt]
    locals: List[bool] = field(default_factory=list)


@dataclass
class IfStmt(Stmt):
    condition: Expr
    then_arm: BlockStmt
    else_arm: Optional[BlockStmt] = None


@dataclass
class WhileStmt(Stmt):
    condition: Expr
    block: BlockStmt


@dataclass
class Upvalue:
    index: int
    is_local: bool


@dataclass
class FuncDecl(Stmt):
    export: bool
    name: Identifier
    args: List[Identifier]
    body: BlockStmt
    upvalues: List[Upvalue] = field(default_factory=list)


@dataclass
class ReturnStmt(Stmt):
    expr: Optional[Expr] = None


@dataclass
class Method(AstNode):
    name: Identifier
    args: List[Identifier]
    body: BlockStmt


@dataclass
class ClassDecl(Stmt):
    export: bool
    name: Identifier
    parent: Optional[Identifier]
    methods: List[Method]


@dataclass
class TryStmt(Stmt):
    try_block: BlockStmt
    catch_name: Identifier
    catch_block: BlockStmt


@dataclass
class ThrowStmt(Stmt):
    expr: Expr


@dataclass
class ForInStmt(Stmt):
    pattern: Pattern
    expr: Expr
    body: BlockStmt


@dataclass
class BreakStmt(Stmt):
    pass


@dataclass
class ContinueStmt(Stmt):
    pass
