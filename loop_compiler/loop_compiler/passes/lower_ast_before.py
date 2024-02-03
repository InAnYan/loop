from loop_compiler.loop_ast.base import *
from loop_compiler.loop_ast.stmt import *
from loop_compiler.loop_ast.expr import *
from loop_compiler.loop_ast.module import *


def lower_before_module(module: Module) -> Module:
    return Module(
        module.path, lower_before_stmt_list(module.stmts), module.globals_count
    )


def lower_before_stmt(stmt: Stmt) -> List[Stmt]:
    match stmt:
        case ImportFromStmt(pos, names, path):
            vars = [
                VarDecl(pos, False, name) for name in names
            ]  # I wanted this to be a Let Decl, but I assign there... That's why this should be after semantic check.
            as_stmt = ImportAsStmt(pos, path, Identifier(pos, "__module"))
            assignments = [
                ExprStmt(
                    pos,
                    Assignment(
                        pos,
                        VarExpr(pos, name),
                        GetAttrExpr(
                            pos, VarExpr(pos, Identifier(pos, "__module")), name
                        ),
                    ),
                )
                for name in names
            ]

            # TODO: This creates copies of values.

            return vars + [BlockStmt(pos, [as_stmt] + assignments)]

        case BlockStmt(pos, stmts):
            return [BlockStmt(pos, lower_before_stmt_list(stmts))]

        case _:
            return [stmt]


def lower_before_stmt_list(stmts: List[Stmt]) -> List[Stmt]:
    return [s for stmt in stmts for s in lower_before_stmt(stmt)]
