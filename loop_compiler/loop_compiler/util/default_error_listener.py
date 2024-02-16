from typing import Optional

from loop_compiler.loop_ast.base import SourcePosition
from loop_compiler.util.error_listener import ErrorListener


class DefaultErrorListener(ErrorListener):
    def error_impl(self, pos: Optional[SourcePosition], msg: str):
        self.msg(pos, "error", msg)

    def note(self, pos: Optional[SourcePosition], msg: str):
        self.msg(pos, "note", msg)

    def msg(self, pos: Optional[SourcePosition], kind: str, msg: str):
        if pos:
            lines = pos.file.contents.split("\n")  # TODO: Very bad.
            print(f"{pos.file.path}:{pos.start.line}: {kind}: {msg}")
            print(
                f" {pos.start.line}|    {lines[pos.start.line - 1]}"
            )  # TODO: And that's bad too.
            print("")
        else:
            print(f"{kind}: {msg}")


class SilentErrorListener(ErrorListener):
    def silent(self, pos: Optional[SourcePosition], msg: str):
        pass

    error_impl = silent
    note = silent
