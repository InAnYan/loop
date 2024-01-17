from typing import List
from error_listener import ErrorListener
from loop_ast import SourcePosition

from repr import LongInst, Opcode, Value


class Emitter:
    error_listener: ErrorListener
    code: List[int]
    constants: List[Value]
    lines: List[int]

    def __init__(self, error_listener: ErrorListener) -> None:
        self.error_listener = error_listener
        self.code = []
        self.constants = []
        self.lines = []

    def opcode(self, opcode: Opcode, pos: SourcePosition):
        self.byte(opcode.value, pos)

    def add_constant(self, value: Value, pos: SourcePosition) -> int:
        self.constants.append(value)
        index = len(self.constants) - 1

        if index > 255:
            self.error_listener.error(pos, "too many constants")

        return index

    def add_and_process_constant(self, value: Value, pos: SourcePosition, op: LongInst):
        index = self.add_constant(value, pos)
        self.long_inst(index, pos, op)

    def long_inst(self, index: int, pos: SourcePosition, op: LongInst):
        match op:
            case LongInst.PushConstant:
                self.long_inst_impl(index, pos, [Opcode.PushConstant])
            case LongInst.DefineGlobal:
                self.long_inst_impl(index, pos, [Opcode.DefineGlobal])
            case LongInst.GetGlobal:
                self.long_inst_impl(index, pos, [Opcode.GetGlobal])
            case LongInst.SetGlobal:
                self.long_inst_impl(index, pos, [Opcode.SetGlobal])

    def long_inst_impl(self, index: int, pos: SourcePosition, lst: List[Opcode]):
        # There is no check for correctness.

        if index < 256:
            self.opcode(lst[0])
            self.byte(index)

    def jump(self, opcode: Opcode, pos: SourcePosition) -> int:
        self.opcode(opcode, pos)
        self.byte(0xFF, pos)
        self.byte(0xFF, pos)

        return len(self.code) - 3

    def patch_jump(self, index: int, pos: SourcePosition):
        assert index < len(self.code)
        assert index + 3 <= len(self.code)

        jump = len(self.code) - index - 3
        if jump > 0xFFFF:
            self.error_listener.error(pos, "jump is too far")

        # TODO: Endianess bug?
        self.code[index + 1] = jump & 0xFF
        self.code[index + 2] = (jump >> 8) & 0xFF

    def byte(self, byte: int, pos: SourcePosition):
        assert type(byte) == int and byte >= 0 and byte < 256

        self.code.append(byte)
        self.lines.append(pos.line)

    def make_json_chunk_object(self) -> dict:
        return {
            "code": self.code,
            "constants": list(map(lambda v: v.make_json_object(), self.constants)),
            "lines": self.lines,
        }
