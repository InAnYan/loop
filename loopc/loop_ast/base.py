from __future__ import annotations

from dataclasses import dataclass
from typing import Optional
from enum import Enum, auto


@dataclass
class File:
    path: str
    contents: str


@dataclass
class SourcePosition:
    file: File
    line: int


@dataclass
class Identifier:
    pos: SourcePosition
    text: str
    ref_type: Optional[RefType] = None
    ref_index: Optional[int] = None


class RefType(Enum):
    GLOBAL = auto()
    EXPORT = auto()
    LOCAL = auto()
    # UPVALUE = auto()


@dataclass
class AstNode:
    pos: SourcePosition


class AstVisitor:
    def visit(self, node: AstNode):
        node_name = type(node).__name__
        fn = getattr(self, "visit_" + node_name, None)
        if fn:
            return fn(node)
        else:
            raise Exception(
                f"Unhandled visiting of {node_name} in {type(self).__name__}"
            )
