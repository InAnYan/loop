from loop_compiler.loop_ast.base import *
from loop_compiler.loop_ast.stmt import *
from loop_compiler.loop_ast.expr import *
from loop_compiler.loop_ast.module import *


def lower_after_module(module: Module) -> Module:
    return Module(
        module.path, lower_after_stmt_list(module.stmts), module.globals_count
    )


def lower_after_stmt(stmt: Stmt) -> List[Stmt]:
    match stmt:
        case LetDecl(pos, export, name, expr):
            return [VarDecl(pos, export, name, expr)]

        case BlockStmt(pos, stmts):
            return [BlockStmt(pos, lower_after_stmt_list(stmts))]

        case _:
            return [stmt]


def lower_after_stmt_list(stmts: List[Stmt]) -> List[Stmt]:
    return [s for stmt in stmts for s in lower_after_stmt(stmt)]
