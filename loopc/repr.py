from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum


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


class Value(ABC):
    @abstractmethod
    def make_json_object(self) -> dict:
        raise NotImplemented()


@dataclass
class NumberValue(Value):
    num: int

    def make_json_object(self) -> dict:
        return {"type": "integer", "data": self.num}


@dataclass
class BoolValue(Value):
    b: bool

    def make_json_object(self) -> dict:
        return {"type": "bool", "data": self.b}
