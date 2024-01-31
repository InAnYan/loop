from dataclasses import dataclass
from typing import List, Optional
import os

from loop_ast.stmt import Stmt


@dataclass
class Module:
    path: str
    stmts: List[Stmt]
    globals_count: Optional[int] = None


def get_module_path(parent_module_path: str, submodule_path: str) -> str:
    return os.path.join(os.path.dirname(parent_module_path), submodule_path)


def get_compiled_path(module_path: str) -> str:
    dir, file = os.path.split(module_path)
    return os.path.join(dir, '.loop_compiled', os.path.splitext(file)[0]) + '.code'


def get_module_name(module_path: str) -> str:
    return os.path.splitext(os.path.basename(module_path))[0]
