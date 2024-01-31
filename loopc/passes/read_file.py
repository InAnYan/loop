from typing import Optional
from loop_ast.base import File


def read_loop_file(path: str) -> Optional[File]:
    try:
        with open(path) as fin:
            source = fin.read()
            return File(path, source)
    except FileNotFoundError:
        return None
