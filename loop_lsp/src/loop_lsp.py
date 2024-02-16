from typing import List, Optional
from loop_compiler.loop_ast.base import File, SourcePoint, SourcePosition
from pygls.server import LanguageServer
from lsprotocol import types as lsp
import pygls
from loop_compiler.full_passes import ErrorListener, str_to_loop_module_checked

server = LanguageServer("loop-lsp", "v0.1")


# To read: https://stackoverflow.com/questions/73595943/how-to-prevent-lsp-ondidchangecontent-event-from-firing-too-many-times


@server.feature(lsp.TEXT_DOCUMENT_DID_CHANGE)
def text_document_did_change(params: lsp.DidChangeTextDocumentParams):
    validate(params.text_document.uri)


@server.feature(lsp.TEXT_DOCUMENT_DID_OPEN)
def text_document_did_open(params: lsp.DidOpenTextDocumentParams):
    validate(params.text_document.uri)


class ValidateErrorListener(ErrorListener):
    errors: List[lsp.Diagnostic]

    def __init__(self) -> None:
        super().__init__()
        self.errors = []

    def error_impl(self, pos: Optional[SourcePosition], msg: str):
        self.errors.append(
            lsp.Diagnostic(
                pos_to_range(pos),
                msg,
                lsp.DiagnosticSeverity.Error,
            )
        )

    def note(self, pos: Optional[SourcePosition], msg: str):
        self.errors.append(
            lsp.Diagnostic(
                pos_to_range(pos),
                msg,
                lsp.DiagnosticSeverity.Information,
            )
        )


def pos_to_range(pos: Optional[SourcePosition]) -> lsp.Range:
    if not pos:
        return lsp.Range(lsp.Position(0, 0), lsp.Position(0, 0))
    return lsp.Range(point_to_pos(pos.start), point_to_pos(pos.end))


def point_to_pos(point: SourcePoint) -> lsp.Position:
    return lsp.Position(point.line - 1, point.col - 1)


def validate(uri: str):
    doc = server.workspace.get_text_document(uri)
    error_listener = ValidateErrorListener()
    str_to_loop_module_checked(error_listener, File(doc.uri[7:], doc.source))
    server.publish_diagnostics(uri, error_listener.errors)


server.start_io()
