from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum, auto


class Opcode(Enum):
    Return = 0
    PushConstant = 1
    Negate = 2
    Add = 3
    Subtract = 4
    Multiply = 5
    Divide = 6
    Print = 7
    Pop = 8
    Plus = 9
    Equal = 10
    Not = 11
    JumpIfFalse = 12
    JumpIfTrue = 13
    PushTrue = 14
    PushFalse = 15
    Greater = 16
    Less = 17
    PushNull = 18
    DefineGlobal = 19
    GetGlobal = 20
    SetGlobal = 21


class LongInst(Enum):
    PushConstant = auto()
    DefineGlobal = auto()
    GetGlobal = auto()
    SetGlobal = auto()


class Value(ABC):
    @abstractmethod
    def make_json_object(self) -> dict:
        raise NotImplemented()


@dataclass
class NumberValue(Value):
    num: int

    def make_json_object(self) -> dict:
        return {"type": "Integer", "data": self.num}


@dataclass
class BoolValue(Value):
    b: bool

    def make_json_object(self) -> dict:
        return {"type": "Boolean", "data": self.b}


@dataclass
class NullValue(Value):
    def make_json_object(self) -> dict:
        return {"type": "Null"}


@dataclass
class StringValue(Value):
    txt: str

    def make_json_object(self) -> dict:
        return {"type": "String", "data": self.txt}
