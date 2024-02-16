from abc import ABC, abstractmethod
from typing import Optional

from loop_compiler.loop_ast.base import SourcePosition


class ErrorListener(ABC):
    had_error: bool

    def __init__(self) -> None:
        self.had_error = False

    def error(self, pos: Optional[SourcePosition], msg: str):
        self.had_error = True
        self.error_impl(pos, msg)

    @abstractmethod
    def error_impl(self, pos: Optional[SourcePosition], msg: str):
        raise NotImplemented()

    @abstractmethod
    def note(self, pos: Optional[SourcePosition], msg: str):
        raise NotImplemented()
