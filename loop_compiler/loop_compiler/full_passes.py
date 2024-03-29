import os
from typing import Iterable, Optional
from loop_compiler.loop_ast.base import File, SourcePosition
from loop_compiler.loop_ast.module import Module, get_compiled_path

from loop_compiler.passes.lower_ast_after import lower_after_module
from loop_compiler.passes.lower_ast_before import lower_before_module
from loop_compiler.passes.read_file import read_loop_file
from loop_compiler.passes.loop_parser import parse_loop_module
from loop_compiler.passes.semantic_check import perform_semantic_check
from loop_compiler.passes.compiler import compile_module
from loop_compiler.passes.write_chunk import write_chunk

from loop_compiler.util.error_listener import ErrorListener


def full_passes(
    error_listener: ErrorListener,
    path: str,
    parent_pos: Optional[SourcePosition] = None,
    **kwargs,
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
        if module := str_to_loop_module_checked(error_listener, file, **kwargs):
            if chunk := compile_module(error_listener, module):
                return write_chunk(compiled, chunk)
    else:
        error_listener.error(parent_pos, f"file not found: '{path}'")

    return False


def str_to_loop_module_checked(
    error_listener: ErrorListener,
    file: File,
    **kwargs,
) -> Optional[Module]:
    if module := parse_loop_module(error_listener, file):
        lowered = lower_before_module(module)

        with new_cd(os.path.dirname(file.path)):
            semantic_pass = perform_semantic_check(
                file, error_listener, lowered, **kwargs
            )

        if semantic_pass:
            lowered = lower_after_module(lowered)
            return lowered


def resolve_path(path: str) -> str:
    for search in generate_search_paths():
        path = os.path.join(search, path)
        if os.path.exists(path):
            return path

    return path


def generate_search_paths() -> Iterable[str]:
    yield ""
    if val := os.environ.get("LOOP_PACKAGES_PATH"):
        yield val
    else:
        yield "/mnt/d/Programming/Loop/packages"  # TODO: Delete


# Source: https://stackoverflow.com/questions/75048986/way-to-temporarily-change-the-directory-in-python-to-execute-code-without-affect
import contextlib


@contextlib.contextmanager
def new_cd(x):
    d = os.getcwd()

    # This could raise an exception, but it's probably
    # best to let it propagate and let the caller
    # deal with it, since they requested x
    if x != "":
        os.chdir(x)

    try:
        yield

    finally:
        # This could also raise an exception, but you *really*
        # aren't equipped to figure out what went wrong if the
        # old working directory can't be restored.
        os.chdir(d)
