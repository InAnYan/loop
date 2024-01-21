import sys
from typing import List, Optional

from util.error_listener import ErrorListener
from full_passes import full_passes
from loop_ast.base import SourcePosition


def main():
    if len(sys.argv) != 2:
        print("error: wrong arguments count")
        print("usage: main.py source_path")
        sys.exit(1)

    in_path = sys.argv[1]
    full_passes(TheErrorListener(), in_path)


class TheErrorListener(ErrorListener):
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


if __name__ == "__main__":
    main()
