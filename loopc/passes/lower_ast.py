from loop_ast.base import *
from loop_ast.stmt import *
from loop_ast.expr import *
from loop_ast.module import *


def lower_module(module: Module) -> Module:
    return Module(module.path, lower_stmt_list(module.stmts))


def lower_stmt(stmt: Stmt) -> List[Stmt]:
    match stmt:
        case ImportFromStmt(pos, names, path):
            vars = [VarDecl(pos, False, name) for name in names]
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
            return [BlockStmt(pos, lower_stmt_list(stmts))]

        case _:
            return [stmt]


def lower_stmt_list(stmts: List[Stmt]) -> List[Stmt]:
    return [s for stmt in stmts for s in lower_stmt(stmt)]
