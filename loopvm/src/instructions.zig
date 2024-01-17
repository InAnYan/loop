const Chunk = @import("chunk.zig").Chunk;

pub const Opcode = enum(u8) { Return = 0, PushConstant, Negate, Add, Subtract, Multiply, Divide, Print, Pop, Plus, Equal, Not, JumpIfFalse, JumpIfTrue, PushTrue, PushFalse, Greater, Less, _ };

pub const SimpleInstruction = struct {
    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, name: []const u8, offset: usize) !void {
        _ = self;
        _ = chunk;
        _ = offset;

        try writer.print("{s}\n", .{name});
    }
};

pub const ConstantInstruction = struct {
    index: u8,

    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, name: []const u8, offset: usize) !void {
        _ = offset;

        const value = try chunk.getConstant(self.index);

        try writer.print("{s:<16} {d:4} '", .{ name, self.index });
        try value.print(writer);
        try writer.writeAll("'\n");
    }
};

pub const JumpInstruction = struct {
    jump: u16,

    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, name: []const u8, offset: usize) !void {
        _ = chunk;

        const new_ip = offset + 3 + self.jump;
        try writer.print("{s:<16} {d:4}\n", .{ name, new_ip });
    }
};

pub const UnknownInstruction = struct {
    opcode: u8,

    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, name: []const u8, offset: usize) !void {
        _ = chunk;
        _ = offset;

        try writer.print("{s:<16} {d:4}\n", .{ name, self.opcode });
    }
};

pub const Instruction = union(enum) {
    Return: SimpleInstruction,
    PushConstant: ConstantInstruction,
    Unknown: UnknownInstruction,
    Negate: SimpleInstruction,
    Add: SimpleInstruction,
    Subtract: SimpleInstruction,
    Multiply: SimpleInstruction,
    Divide: SimpleInstruction,
    Print: SimpleInstruction,
    Pop: SimpleInstruction,
    Plus: SimpleInstruction,
    Equal: SimpleInstruction,
    Not: SimpleInstruction,
    JumpIfFalse: JumpInstruction,
    JumpIfTrue: JumpInstruction,
    PushTrue: SimpleInstruction,
    PushFalse: SimpleInstruction,
    Greater: SimpleInstruction,
    Less: SimpleInstruction,

    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, offset: usize) !void {
        try writer.print("{d:0>4} ", .{offset});

        const optional_line = try chunk.getLine(offset);
        if (optional_line) |line| {
            try writer.print("{d:<4} ", .{line});
        } else {
            try writer.writeAll("| ");
        }

        switch (self.*) {
            .Return => {
                try self.Return.disassemble(writer, chunk, "Return", offset);
            },

            .PushConstant => {
                try self.PushConstant.disassemble(writer, chunk, "PushConstant", offset);
            },

            .Negate => {
                try self.Negate.disassemble(writer, chunk, "Negate", offset);
            },

            .Add => {
                try self.Add.disassemble(writer, chunk, "Add", offset);
            },

            .Subtract => {
                try self.Subtract.disassemble(writer, chunk, "Subtract", offset);
            },

            .Multiply => {
                try self.Multiply.disassemble(writer, chunk, "Multiply", offset);
            },

            .Divide => {
                try self.Divide.disassemble(writer, chunk, "Divide", offset);
            },

            .Print => {
                try self.Print.disassemble(writer, chunk, "Print", offset);
            },

            .Pop => {
                try self.Pop.disassemble(writer, chunk, "Pop", offset);
            },

            .Plus => {
                try self.Plus.disassemble(writer, chunk, "Plus", offset);
            },

            .Equal => {
                try self.Equal.disassemble(writer, chunk, "Equal", offset);
            },

            .Not => {
                try self.Not.disassemble(writer, chunk, "Not", offset);
            },

            .JumpIfFalse => {
                try self.JumpIfFalse.disassemble(writer, chunk, "JumpIfFalse", offset);
            },

            .JumpIfTrue => {
                try self.JumpIfTrue.disassemble(writer, chunk, "JumpIfTrue", offset);
            },

            .Unknown => {
                try self.Unknown.disassemble(writer, chunk, "Unknown", offset);
            },

            .PushTrue => {
                try self.PushTrue.disassemble(writer, chunk, "PushTrue", offset);
            },

            .PushFalse => {
                try self.PushFalse.disassemble(writer, chunk, "PushFalse", offset);
            },

            .Greater => {
                try self.Greater.disassemble(writer, chunk, "Greater", offset);
            },

            .Less => {
                try self.Less.disassemble(writer, chunk, "Less", offset);
            },
        }
    }
};
