from abc import ABC, abstractmethod

from loop_ast import SourcePosition


class ErrorListener(ABC):
    had_error: bool
    
    def __init__(self) -> None:
        self.had_error = False

    def error(self, pos: SourcePosition, msg: str):
        self.had_error
        self.error_impl(pos, msg)

    @abstractmethod
    def error_impl(self, pos: SourcePosition, msg: str):
        raise NotImplemented() 
