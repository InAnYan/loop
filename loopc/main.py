import sys
import json

from compiler import ModuleCompiler
from error_listener import ErrorListener
from loop_ast import SourcePosition
from loop_parser import parse_loop_module


def main():
    if len(sys.argv) != 3:
        print("error: wrong arguments count")
        print("usage: main.py source_path output_path")
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]

    error_listener = TheErrorListener()

    with open(in_path) as fin:
        source = fin.read()
        module = parse_loop_module(error_listener, in_path, source)
        if not module:
            return
        compiler = ModuleCompiler(error_listener)
        res = compiler.compile(module)
        if res:
            with open(out_path, "w") as fout:
                fout.write(json.dumps(res))


class TheErrorListener(ErrorListener):
    def error_impl(self, pos: SourcePosition, msg: str):
        print(f"{pos.path}:{pos.line}: error: {msg}.")


if __name__ == "__main__":
    main()
