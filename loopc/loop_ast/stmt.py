from dataclasses import dataclass
from typing import List, Optional

from loop_ast.base import *
from loop_ast.expr import Expr


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
    name: Identifier
    expr: Optional[Expr] = None


@dataclass
class BlockStmt(Stmt):
    stmts: List[Stmt]
    locals_count: Optional[int] = None


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
class FuncDecl(Stmt):
    export: bool
    name: Identifier
    args: List[Identifier]
    body: BlockStmt


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
    methods: List[Method]
