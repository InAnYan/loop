from abc import ABC, abstractmethod
from typing import Optional

from loop_ast.base import SourcePosition


class ErrorListener(ABC):
    had_error: bool

    def __init__(self) -> None:
        self.had_error = False

    def error(self, pos: SourcePosition, msg: str):
        self.had_error = True
        self.error_impl(pos, msg)

    @abstractmethod
    def error_no_pos(self, msg: str, path: Optional[str] = None):
        raise NotImplemented()

    @abstractmethod
    def error_impl(self, pos: SourcePosition, msg: str):
        raise NotImplemented()

    @abstractmethod
    def note(self, pos: SourcePosition, msg: str):
        raise NotImplemented()
