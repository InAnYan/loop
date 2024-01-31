from typing import Iterable, Optional

from loop_ast.module import *

from passes.read_file import read_loop_file
from passes.loop_parser import parse_loop_module
from passes.semantic_check import perform_semantic_check
from passes.lower_ast import lower_module
from passes.compiler import compile_module
from passes.write_chunk import write_chunk

from util.error_listener import ErrorListener


def full_passes(
    error_listener: ErrorListener, path: str, parent_path: Optional[str] = None
) -> Optional[bool]:
    resolved = resolve_path(path)
    compiled = get_compiled_path(resolved)
    # TODO: Circular import fix.
    if os.path.exists(compiled) and os.path.getmtime(compiled) < os.path.getmtime(
        compiled
    ):
        return True

    # print(f"FULL CurDir: {os.getcwd()} Path: {path} | Resolved: {resolved} | Compiled: {compiled}")

    if file := read_loop_file(resolved):
        if module := parse_loop_module(error_listener, file):
            lowered = lower_module(module)

            with new_cd(os.path.dirname(resolved)):
                semantic_pass = perform_semantic_check(file, error_listener, lowered)
            
            if semantic_pass:
                if chunk := compile_module(error_listener, lowered):
                    return write_chunk(compiled, chunk)
                else:
                    error_listener.error_no_pos(
                        f"failed to compile: '{resolved}'", parent_path
                    )
            else:
                error_listener.error_no_pos(
                    f"failed semantic check: '{resolved}'", parent_path
                )
        else:
            error_listener.error_no_pos(f"failed to parse: '{resolved}'", parent_path)
    else:
        error_listener.error_no_pos(f"file not found: '{path}'", parent_path)

    return False


def resolve_path(path: str) -> str:
    for search in generate_search_paths():
        path = os.path.join(search, path)
        if os.path.exists(path):
            return path

    return path


def generate_search_paths() -> Iterable[str]:
    yield ''
    if val := os.environ.get('LOOP_PACKAGES_PATH'):
        yield val


# Source: https://stackoverflow.com/questions/75048986/way-to-temporarily-change-the-directory-in-python-to-execute-code-without-affect
import contextlib


@contextlib.contextmanager
def new_cd(x):
    d = os.getcwd()

    # This could raise an exception, but it's probably
    # best to let it propagate and let the caller
    # deal with it, since they requested x
    if x != '':
        os.chdir(x)

    try:
        yield

    finally:
        # This could also raise an exception, but you *really*
        # aren't equipped to figure out what went wrong if the
        # old working directory can't be restored.
        os.chdir(d)