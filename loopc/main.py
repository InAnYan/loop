import sys
import json

from compiler import ModuleCompiler
from error_listener import ErrorListener
from loop_ast.base import SourcePosition
from loop_ast.module import *
from loop_parser import parse_loop_module
from semantic_check import SemanticCheck
from repr import FunctionValue


def main():
    if len(sys.argv) != 3:
        print("error: wrong arguments count")
        print("usage: main.py source_path output_path")
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]

    with open(in_path) as fin:
        source = fin.read()
        error_listener = TheErrorListener(source)
        module = parse_loop_module(error_listener, in_path, source)
        if not module:
            return
        semantic = SemanticCheck(error_listener, module)
        semantic.check(module)

        if error_listener.had_error:
            return

        compiler = ModuleCompiler(error_listener)
        res = compiler.do(module)
        if res:
            with open(out_path, "w") as fout:
                fout.write(
                    json.dumps(
                        FunctionValue(
                            f"<script: {in_path}", 0, res
                        ).make_json_object_data()
                    )
                )


class TheErrorListener(ErrorListener):
    lines: List[str]

    def __init__(self, source: str) -> None:
        super().__init__()
        self.lines = source.split("\n")

    def error_impl(self, pos: SourcePosition, msg: str):
        self.msg(pos, "error", msg)

    def note(self, pos: SourcePosition, msg: str):
        self.msg(pos, "note", msg)

    def msg(self, pos: SourcePosition, kind: str, msg: str):
        print(f"{pos.path}:{pos.line}: {kind}: {msg}")
        print(f" {pos.line}|    {self.lines[pos.line - 1]}")
        print("")


if __name__ == "__main__":
    main()
