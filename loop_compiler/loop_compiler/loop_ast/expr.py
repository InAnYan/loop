from dataclasses import dataclass
from enum import Enum, auto
from typing import List
from loop_compiler.loop_ast.base import AstNode, Identifier

from loop_compiler.loop_ast.repr import Opcode


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
    null: None = None


@dataclass
class Assignment(Expr):
    var: Expr
    expr: Expr


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

    Is = auto()

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

            case self.Is:
                return Opcode.InstanceOf


@dataclass
class BinaryOp(Expr):
    op: BinaryOpType
    left: Expr
    right: Expr


@dataclass
class CallExpr(Expr):
    callee: Expr
    args: List[Expr]


@dataclass
class GetAttrExpr(Expr):
    obj: Expr
    attr: Identifier


@dataclass
class GetItemExpr(Expr):
    obj: Expr
    index: List[Expr]


@dataclass
class DictionaryPair(AstNode):
    key: Expr
    value: Expr


@dataclass
class DictionaryLiteral(Expr):
    entries: List[DictionaryPair]


@dataclass
class ListLiteral(Expr):
    elements: List[Expr]
