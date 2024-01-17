const std = @import("std");
const VirtualMachine = @import("virtual_machine.zig").VirtualMachine;
const Opcode = @import("instructions.zig").Opcode;
const Value = @import("value.zig").Value;
const Instruction = @import("instructions.zig").Instruction;
const RuntimeError = @import("errors.zig").RuntimeError;
const StringObject = @import("objects.zig").StringObject;

pub const Chunk = struct {
    code: std.ArrayList(u8),
    constants: std.ArrayList(Value),
    lines: std.ArrayList(usize),

    const Self = @This();

    pub fn init(vm: *VirtualMachine) Self {
        return .{ .code = std.ArrayList(u8).init(vm.getAllocator()), .constants = std.ArrayList(Value).init(vm.getAllocator()), .lines = std.ArrayList(usize).init(vm.getAllocator()) };
    }

    pub fn fromBuffer(vm: *VirtualMachine, buf: []const u8) !Self {
        const json = try std.json.parseFromSlice(std.json.Value, vm.getAllocator(), buf, .{});
        defer json.deinit();

        const object = json.value.object;

        var chunk = Self.init(vm);

        for (object.get("code").?.array.items) |elem| {
            try chunk.code.append(@intCast(elem.integer));
        }

        for (object.get("constants").?.array.items) |elem| {
            try chunk.constants.append(try Value.fromJSON(vm, elem));
        }

        for (object.get("lines").?.array.items) |elem| {
            try chunk.lines.append(@intCast(elem.integer)); // NOTE: Potential bug because of @as.
        }

        return chunk;
    }

    pub fn deinit(self: *Self) void {
        self.code.deinit();
        self.constants.deinit();
        self.lines.deinit();
    }

    pub const ReadResult = struct {
        inst: Instruction,
        offset: usize,
    };

    pub fn readAt(self: *const Self, offset: usize) !ReadResult {
        const opcode = try self.getOpcode(offset);
        switch (opcode) {
            .Return => {
                return .{ .inst = .{ .Return = .{} }, .offset = offset + 1 };
            },

            .PushConstant => {
                const index = try self.getByte(offset + 1);
                return .{ .inst = .{ .PushConstant = .{ .index = index } }, .offset = offset + 2 };
            },

            .Negate => {
                return .{ .inst = .{ .Negate = .{} }, .offset = offset + 1 };
            },

            .Add => {
                return .{ .inst = .{ .Add = .{} }, .offset = offset + 1 };
            },

            .Subtract => {
                return .{ .inst = .{ .Subtract = .{} }, .offset = offset + 1 };
            },

            .Multiply => {
                return .{ .inst = .{ .Multiply = .{} }, .offset = offset + 1 };
            },

            .Divide => {
                return .{ .inst = .{ .Divide = .{} }, .offset = offset + 1 };
            },

            .Print => {
                return .{ .inst = .{ .Print = .{} }, .offset = offset + 1 };
            },

            .Pop => {
                return .{ .inst = .{ .Pop = .{} }, .offset = offset + 1 };
            },

            .Plus => {
                return .{ .inst = .{ .Plus = .{} }, .offset = offset + 1 };
            },

            .Equal => {
                return .{ .inst = .{ .Equal = .{} }, .offset = offset + 1 };
            },

            .Not => {
                return .{ .inst = .{ .Not = .{} }, .offset = offset + 1 };
            },

            .JumpIfFalse => {
                const part_1 = try self.getByte(offset + 1);
                const part_2 = try self.getByte(offset + 2);

                const arg = @shlExact(@as(u16, part_2), 8) | part_1;

                return .{ .inst = .{ .JumpIfFalse = .{ .jump = arg } }, .offset = offset + 3 };
            },

            .JumpIfTrue => {
                const part_1 = try self.getByte(offset + 1);
                const part_2 = try self.getByte(offset + 2);

                const arg = @shlExact(@as(u16, part_2), 8) | part_1;

                return .{ .inst = .{ .JumpIfTrue = .{ .jump = arg } }, .offset = offset + 3 };
            },

            .PushTrue => {
                return .{ .inst = .{ .PushTrue = .{} }, .offset = offset + 1 };
            },

            .PushFalse => {
                return .{ .inst = .{ .PushFalse = .{} }, .offset = offset + 1 };
            },

            .Greater => {
                return .{ .inst = .{ .Greater = .{} }, .offset = offset + 1 };
            },

            .Less => {
                return .{ .inst = .{ .Less = .{} }, .offset = offset + 1 };
            },

            .PushNull => {
                return .{ .inst = .{ .PushNull = .{} }, .offset = offset + 1 };
            },

            .DefineGlobal => {
                const index = try self.getByte(offset + 1);
                return .{ .inst = .{ .DefineGlobal = .{ .index = index } }, .offset = offset + 2 };
            },

            .GetGlobal => {
                const index = try self.getByte(offset + 1);
                return .{ .inst = .{ .GetGlobal = .{ .index = index } }, .offset = offset + 2 };
            },

            .SetGlobal => {
                const index = try self.getByte(offset + 1);
                return .{ .inst = .{ .SetGlobal = .{ .index = index } }, .offset = offset + 2 };
            },

            _ => {
                return .{ .inst = .{ .Unknown = .{ .opcode = @intFromEnum(opcode) } }, .offset = offset + 1 };
            },
        }
    }

    pub fn getByte(self: *const Self, offset: usize) !u8 {
        if (offset >= self.code.items.len) {
            return RuntimeError.ReadingPastTheChunk;
        }

        return self.code.items[offset];
    }

    pub fn getOpcode(self: *const Self, offset: usize) !Opcode {
        return @enumFromInt(try self.getByte(offset));
    }

    pub fn getConstant(self: *const Self, index: usize) !Value {
        if (index >= self.constants.items.len) {
            return RuntimeError.ConstantDoesNotExists;
        }

        return self.constants.items[index];
    }

    pub fn getStringConstant(self: *const Self, index: usize) !*StringObject {
        const val = try self.getConstant(index);

        if (val == .Object and val.Object.kind == .String) {
            return val.Object.as(StringObject);
        } else {
            // Actually, if the compiler is correct, then this error will never be raised.
            return RuntimeError.WrongType;
        }
    }

    pub fn getLine(self: *const Self, offset: usize) !?usize {
        if (offset >= self.lines.items.len) {
            return null;
        }

        return self.lines.items[offset];
    }

    pub fn disassemble(self: *const Self, writer: anytype, name: []const u8) !void {
        try writer.print("== {s} ==\n", .{name});

        var offset: usize = 0;
        while (offset < self.code.items.len) {
            offset = try self.disassembleInstruction(writer, offset);
        }
    }

    pub fn disassembleInstruction(self: *const Self, writer: anytype, offset: usize) !usize {
        const res = try self.readAt(offset);
        try res.inst.disassemble(writer, self, offset);
        return res.offset;
    }
};
