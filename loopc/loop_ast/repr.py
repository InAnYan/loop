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
    BuildList = 19
    GetGlobal = 20
    SetGlobal = 21
    GetLocal = 22
    SetLocal = 23
    JumpIfFalsePop = 24
    Jump = 25
    Loop = 26
    Call = 27
    Export = 28
    Import = 29
    Top = 30
    GetAttribute = 31
    ModuleEnd = 32
    BuildDictionary = 33
    GetItem = 34
    SetItem = 35
    SetAttribute = 36
    GetExport = 37
    SetExport = 38
    BuildClosure = 39
    GetUpvalue = 40
    SetUpvalue = 41
    CloseUpvalue = 42


class LongInst(Enum):
    PushConstant = auto()
    Import = auto()
    GetAttribute = auto()
    SetAttribute = auto()
    Export = auto()
    GetExport = auto()
    SetExport = auto()


class Value(ABC):
    def make_json_object(self) -> Dict[str, Any]:
        return {
            "type": self.get_type(),
            "data": self.make_json_object_data(),
        }

    @abstractmethod
    def get_type(self) -> str:
        raise NotImplementedError()

    @abstractmethod
    def make_json_object_data(self) -> Any:
        raise NotImplemented()


@dataclass
class Chunk:
    code: List[int]
    constants: List[Value]
    lines: List[int]

    def get_type(self) -> str:
        return "Chunk"

    def make_json_object_data(self) -> Any:
        return {
            "code": self.code,
            "constants": list(map(lambda v: v.make_json_object(), self.constants)),
            "lines": self.lines,
        }


@dataclass
class IntegerValue(Value):
    num: int

    def get_type(self) -> str:
        return "Integer"

    def make_json_object_data(self) -> Any:
        return self.num


@dataclass
class BoolValue(Value):
    b: bool

    def get_type(self) -> str:
        return "Boolean"

    def make_json_object_data(self) -> Any:
        return self.b


@dataclass
class NullValue(Value):
    def get_type(self) -> str:
        return "Null"

    def make_json_object(self) -> Any:
        return None


@dataclass
class StringValue(Value):
    txt: str

    def get_type(self) -> str:
        return "String"

    def make_json_object_data(self) -> Any:
        return self.txt


@dataclass
class FunctionValue(Value):
    name: str
    arity: int
    body: Chunk

    def get_type(self) -> str:
        return "Function"

    def make_json_object_data(self) -> Any:
        return {
            "name": self.name,
            "arity": self.arity,
            "chunk": self.body.make_json_object_data(),
        }


@dataclass
class ModuleValue(Value):
    globals_count: int
    script: Chunk

    def get_type(self) -> str:
        return "Module"

    def make_json_object_data(self) -> Any:
        return {
            "globals_count": self.globals_count,
            "chunk": self.script.make_json_object_data(),
        }


@dataclass
class ClassValue(Value):
    name: str
    methods: List[FunctionValue]

    def get_type(self) -> str:
        return "Class"

    def make_json_object_data(self) -> Any:
        return {
            "name": self.name,
            "methods": list(map(lambda v: v.make_json_object(), self.methods)),
        }
