from dataclasses import dataclass
from enum import Enum, auto
from typing import List, Optional

from repr import Opcode


@dataclass
class SourcePosition:
    path: str
    line: int


@dataclass
class Identifier:
    pos: SourcePosition
    text: str


@dataclass
class AstNode:
    pos: SourcePosition


@dataclass
class Expr(AstNode):
    pass


@dataclass
class IntegerLiteral(Expr):
    num: int


@dataclass
class BoolLiteral(Expr):
    b: bool


@dataclass
class VarExpr(Expr):
    name: Identifier


@dataclass
class StringLiteral(Expr):
    text: str


@dataclass
class NullLiteral(Expr):
    pass


class UnaryOpType(Enum):
    Plus = auto()
    Negate = auto()
    Not = auto()

    def to_opcode(self) -> Opcode:
        match self:
            case self.Plus:
                return Opcode.Plus
            case self.Negate:
                return Opcode.Negate
            case self.Not:
                return Opcode.Not


@dataclass
class UnaryOp(Expr):
    op: UnaryOpType
    expr: Expr


class BinaryOpType(Enum):
    LogicalOr = auto()
    LogicalAnd = auto()

    Equal = auto()
    NotEqual = auto()

    Greater = auto()
    Less = auto()
    GreaterEqual = auto()
    LessEqual = auto()

    Add = auto()
    Subtract = auto()
    Multiply = auto()
    Divide = auto()

    def to_opcode(self) -> Opcode:
        match self:
            case self.LogicalOr:
                raise Exception("logical or has special semantics")
            case self.LogicalAnd:
                raise Exception("logical and has special semantics")

            case self.Equal:
                return Opcode.Equal
            case self.NotEqual:
                # Don't forget the Opcode.Not after Opcode.Equal.
                return Opcode.Equal

            case self.Greater:
                return Opcode.Greater
            case self.Less:
                return Opcode.Less
            case self.GreaterEqual:
                # Don't forget the Opcode.Not after Opcode.Less.
                return Opcode.Less
            case self.LessEqual:
                # Don't forget the Opcode.Not after Opcode.Greater.
                return Opcode.Greater

            case self.Add:
                return Opcode.Add
            case self.Subtract:
                return Opcode.Subtract
            case self.Multiply:
                return Opcode.Multiply
            case self.Divide:
                return Opcode.Divide


@dataclass
class BinaryOp(Expr):
    op: BinaryOpType
    left: Expr
    right: Expr


@dataclass
class Stmt(AstNode):
    pass


@dataclass
class PrintStmt(Stmt):
    expr: Expr


@dataclass
class ExprStmt(Stmt):
    expr: Expr


@dataclass
class VarDecl(Stmt):
    name: Identifier
    expr: Optional[Expr] = None


@dataclass
class Module:
    path: str
    stmts: List[Stmt]
