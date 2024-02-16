from typing import Sequence
from loop_compiler.loop_ast.base import *
from loop_compiler.loop_ast.loop_patterns import IdentifierPattern, ListPattern
from loop_compiler.loop_ast.stmt import *
from loop_compiler.loop_ast.expr import *
from loop_compiler.loop_ast.module import *


def lower_before_module(module: Module) -> Module:
    return Module(
        module.path, lower_before_stmt_list(module.stmts), module.globals_count
    )


global_counter = 0


def make_unique_int() -> int:
    global global_counter
    global_counter += 1
    return global_counter


def lower_before_stmt(stmt: Stmt) -> List[Stmt]:
    match stmt:
        case ImportFromStmt(pos, names, path):
            # NOTE: IT IS THERE BECAUSE IT CREATES NEW VARIABLES, AND THEY SHOULD BE RESOLVED BY SEMANTIC CHECK.

            module = Identifier(pos, "__module" + str(make_unique_int()))
            as_stmt = ImportAsStmt(pos, path, module)
            vars = [
                LetDecl(
                    pos,
                    False,
                    IdentifierPattern(name.pos, name),
                    GetAttrExpr(pos, VarExpr(pos, module), name),
                )
                for name in names
            ]

            # NOTE: This creates copies of values.

            return [as_stmt] + vars

        case BlockStmt(pos, stmts):
            return [BlockStmt(pos, lower_before_stmt_list(stmts))]

        case FuncDecl():
            return [
                FuncDecl(
                    stmt.pos,
                    stmt.export,
                    stmt.name,
                    stmt.args,
                    lower_before_stmt(stmt.body)[0],
                )
            ]

        case Method():
            return [
                Method(
                    stmt.pos,
                    stmt.name,
                    stmt.args,
                    lower_before_stmt(stmt.body)[0],  # TODO: Not really good typing.
                )
            ]

        case ClassDecl():
            return [
                ClassDecl(
                    stmt.pos,
                    stmt.export,
                    stmt.name,
                    stmt.parent,
                    [lower_before_stmt(method)[0] for method in stmt.methods],
                )
            ]

        case VarDecl():
            match stmt.pattern:
                case IdentifierPattern():
                    return [stmt]

                case ListPattern():
                    main_var = Identifier(
                        stmt.pattern.pos, "__mainVar" + str(make_unique_int())
                    )
                    main_decl = LetDecl(
                        stmt.pattern.pos,
                        stmt.export,
                        IdentifierPattern(stmt.pattern.pos, main_var),
                    )
                    vars = [
                        VarDecl(
                            name.pos,
                            stmt.export,
                            name,
                            GetItemExpr(
                                name.pos,
                                VarExpr(name.pos, main_var),
                                [IntegerLiteral(name.pos, i)],
                            ),
                        )
                        for i, name in enumerate(stmt.pattern.patterns)
                    ]

                    return lower_before_stmt_list([main_decl] + vars)

                case _:
                    raise NotImplementedError()

        case ForInStmt():
            match stmt.pattern:
                case IdentifierPattern():
                    return [stmt]

                case ListPattern():
                    main_var = Identifier(
                        stmt.pattern.pos, "__mainVar" + str(make_unique_int())
                    )
                    return lower_before_stmt(
                        ForInStmt(
                            stmt.pos,
                            IdentifierPattern(stmt.pattern.pos, main_var),
                            stmt.expr,
                            BlockStmt(
                                stmt.body.pos,
                                [
                                    LetDecl(
                                        pat.pos,
                                        False,
                                        pat,
                                        GetItemExpr(
                                            pat.pos,
                                            VarExpr(pat.pos, main_var),
                                            [IntegerLiteral(pat.pos, i)],
                                        ),
                                    )
                                    for i, pat in enumerate(stmt.pattern.patterns)
                                ]
                                + stmt.body.stmts,
                            ),
                        )
                    )

                case _:
                    raise NotImplementedError()

        case _:
            return [stmt]


def lower_before_stmt_list(stmts: List[Stmt]) -> List[Stmt]:
    return [s for stmt in stmts for s in lower_before_stmt(stmt)]
