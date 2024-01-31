from typing import Optional

from loop_ast.base import SourcePosition
from util.error_listener import ErrorListener


class DefaultErrorListener(ErrorListener):
    def error_impl(self, pos: SourcePosition, msg: str):
        self.msg(pos, "error", msg)

    def error_no_pos(self, msg: str, path: Optional[str] = None):
        if path:
            print(f"{path}: error: {msg}")
        else:
            print(f"error: {msg}")

    def note(self, pos: SourcePosition, msg: str):
        self.msg(pos, "note", msg)

    def msg(self, pos: SourcePosition, kind: str, msg: str):
        lines = pos.file.contents.split("\n")  # TODO: Very bad.
        print(f"{pos.file.path}:{pos.line}: {kind}: {msg}")
        print(f" {pos.line}|    {lines[pos.line - 1]}")
        print("")


def silent(*args):
    pass


class SilentErrorListener(ErrorListener):
    error_impl = silent
    error_no_pos = silent
    note = silent
