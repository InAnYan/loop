from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum, auto
from typing import Any, Dict, List


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
    GetLocal = 22
    SetLocal = 23
    JumpIfFalsePop = 24
    Jump = 25
    Loop = 26
    Call = 27


class LongInst(Enum):
    PushConstant = auto()
    DefineGlobal = auto()
    GetGlobal = auto()
    SetGlobal = auto()


class Value(ABC):
    @abstractmethod
    def make_json_object(self) -> Dict[str, Any]:
        raise NotImplemented()


@dataclass
class Chunk:
    code: List[int]
    constants: List[Value]
    lines: List[int]

    def make_json_object(self) -> Dict[str, Any]:
        return {
            "code": self.code,
            "constants": list(map(lambda v: v.make_json_object(), self.constants)),
            "lines": self.lines,
        }


@dataclass
class NumberValue(Value):
    num: int

    def make_json_object(self) -> Dict[str, Any]:
        return {"type": "Integer", "data": self.num}


@dataclass
class BoolValue(Value):
    b: bool

    def make_json_object(self) -> Dict[str, Any]:
        return {"type": "Boolean", "data": self.b}


@dataclass
class NullValue(Value):
    def make_json_object(self) -> Dict[str, Any]:
        return {"type": "Null"}


@dataclass
class StringValue(Value):
    txt: str

    def make_json_object(self) -> Dict[str, Any]:
        return {"type": "String", "data": self.txt}


@dataclass
class FunctionValue(Value):
    name: str
    arity: int
    body: Chunk

    def make_json_object(self) -> Dict[str, Any]:
        return {
            "type": "Function",
            "data": self.make_json_object_data(),
        }

    def make_json_object_data(self) -> dict:
        return {
            "name": self.name,
            "arity": self.arity,
            "chunk": self.body.make_json_object(),
        }
