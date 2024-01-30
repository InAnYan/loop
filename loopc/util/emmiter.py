from typing import List
from util.error_listener import ErrorListener
from loop_ast.base import SourcePosition

from loop_ast.repr import Chunk, LongInst, Opcode, Value


class Emitter:
    error_listener: ErrorListener
    code: List[int]
    constants: List[Value]
    lines: List[int]

    def __init__(self, error_listener: ErrorListener) -> None:
        self.error_listener = error_listener
        self.code = []
        self.constants = []
        self.lines = [0]

    def get_pos(self) -> int:
        return len(self.code)

    def opcode(self, opcode: Opcode, pos: SourcePosition):
        self.byte(opcode.value, pos)

    def add_constant(self, value: Value, pos: SourcePosition) -> int:
        for i, const in enumerate(self.constants):
            if const == value:
                return i

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
            case LongInst.Import:
                self.long_inst_impl(index, pos, [Opcode.Import])
            case LongInst.GetAttribute:
                self.long_inst_impl(index, pos, [Opcode.GetAttribute])
            case LongInst.SetAttribute:
                self.long_inst_impl(index, pos, [Opcode.SetAttribute])
            case LongInst.Export:
                self.long_inst_impl(index, pos, [Opcode.Export])
            case LongInst.GetExport:
                self.long_inst_impl(index, pos, [Opcode.GetExport])
            case LongInst.SetExport:
                self.long_inst_impl(index, pos, [Opcode.SetExport])
            case _:
                raise Exception("not implemented long_inst")

    def long_inst_impl(self, index: int, pos: SourcePosition, lst: List[Opcode]):
        # There is no check for correctness.

        if index < 256:
            self.opcode(lst[0], pos)
            self.byte(index, pos)

    def jump(self, opcode: Opcode, pos: SourcePosition) -> int:
        self.opcode(opcode, pos)
        self.byte(0xFF, pos)
        self.byte(0xFF, pos)

        return len(self.code) - 3

    def loop(self, index: int, pos: SourcePosition):
        # TODO: Generalize with patch_jump?
        jump = len(self.code) - index + 3
        if jump > 0xFFFF:
            self.error_listener.error(pos, "jump is too far")

        self.opcode(Opcode.Loop, pos)
        self.byte(jump & 0xFF, pos)
        self.byte((jump >> 8) & 0xFF, pos)

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

        if pos.line == len(self.lines) - 1:
            self.lines[-1] += 1
        else:
            self.lines.append(0)

    def make_chunk(self) -> Chunk:
        return Chunk(self.code, self.constants, self.lines)
