import sys
from subprocess import DEVNULL, Popen, PIPE
from typing import Optional

from loop_compiler.loop_ast.module import get_compiled_path
from loop_compiler.util.default_error_listener import *
from loop_compiler.full_passes import full_passes


def run_and_compile_loop(path: str, **kwargs) -> Optional[Popen]:
    if not full_passes(
        SilentErrorListener() if kwargs.get("silent_el") else DefaultErrorListener(),
        path + ".loop",
        compile_imported=True,
    ):
        return None

    # https://thraxil.org/users/anders/posts/2008/03/13/Subprocess-Hanging-PIPE-is-your-enemy/

    p = Popen(
        ["loopvm", path],
        stdout=PIPE if kwargs.get("pipe_out") else None,
        stderr=DEVNULL if kwargs.get("silent_stderr") else None,
    )

    p.wait()
    return p


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("error: wrong arguments count")
        print("usage: main.py source_path")
        sys.exit(2)

    in_path = sys.argv[1]
    if p := run_and_compile_loop(in_path):
        sys.exit(p.returncode)
    else:
        sys.exit(3)
