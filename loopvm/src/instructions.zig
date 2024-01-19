const Chunk = @import("chunk.zig").Chunk;

pub const Opcode = enum(u8) { Return = 0, PushConstant, Negate, Add, Subtract, Multiply, Divide, Print, Pop, Plus, Equal, Not, JumpIfFalse, JumpIfTrue, PushTrue, PushFalse, Greater, Less, PushNull, DefineGlobal, GetGlobal, SetGlobal, GetLocal, SetLocal, JumpIfFalsePop, Jump, Loop, _ };

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

pub const ByteInstruction = struct {
    byte: u8,

    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, name: []const u8, offset: usize) !void {
        _ = offset;
        _ = chunk;
        try writer.print("{s:<16} {d:4}\n", .{ name, self.byte });
    }
};

pub const LoopInstruction = struct {
    jump: u16,

    const Self = @This();

    pub fn disassemble(self: *const Self, writer: anytype, chunk: *const Chunk, name: []const u8, offset: usize) !void {
        _ = chunk;

        const new_ip = offset - self.jump + 3;
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
    PushNull: SimpleInstruction,
    DefineGlobal: ConstantInstruction,
    GetGlobal: ConstantInstruction,
    SetGlobal: ConstantInstruction,
    GetLocal: ByteInstruction,
    SetLocal: ByteInstruction,
    JumpIfFalsePop: JumpInstruction,
    Jump: JumpInstruction,
    Loop: LoopInstruction,

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

            .PushNull => {
                try self.PushNull.disassemble(writer, chunk, "PushNull", offset);
            },

            .DefineGlobal => {
                try self.DefineGlobal.disassemble(writer, chunk, "DefineGlobal", offset);
            },

            .GetGlobal => {
                try self.GetGlobal.disassemble(writer, chunk, "GetGlobal", offset);
            },

            .SetGlobal => {
                try self.SetGlobal.disassemble(writer, chunk, "SetGlobal", offset);
            },

            .GetLocal => {
                try self.GetLocal.disassemble(writer, chunk, "GetLocal", offset);
            },

            .SetLocal => {
                try self.SetLocal.disassemble(writer, chunk, "SetLocal", offset);
            },

            .JumpIfFalsePop => {
                try self.JumpIfFalsePop.disassemble(writer, chunk, "JumpIfFalsePop", offset);
            },

            .Jump => {
                try self.Jump.disassemble(writer, chunk, "Jump", offset);
            },

            .Loop => {
                try self.Loop.disassemble(writer, chunk, "Loop", offset);
            },
        }
    }
};
