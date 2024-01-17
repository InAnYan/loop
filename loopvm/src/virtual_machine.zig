const std = @import("std");
const memory_manager = @import("memory_manager.zig");
const MemoryManager = memory_manager.MemoryManager;
const MemoryManagerConfiguration = memory_manager.MemoryManagerConfiguration;
const Value = @import("value.zig").Value;
const ValueType = @import("value.zig").ValueType;
const Chunk = @import("chunk.zig").Chunk;
const instructions = @import("instructions.zig");
const Opcode = instructions.Opcode;
const Instruction = instructions.Instruction;
const RuntimeError = @import("errors.zig").RuntimeError;
const objects = @import("objects.zig");
const Object = objects.Object;
const ObjectType = objects.ObjectType;
const StringObject = objects.StringObject;

pub const VirtualMachineConfiguration = struct {
    user_output: std.fs.File.Writer,
    debug_output: std.fs.File.Writer,
    trace_execution: bool,
    call_frame_count: usize,
    stack_size: usize,
};

const HASH_MAP_LOAD: u64 = 80;

pub const VirtualMachine = struct {
    conf: VirtualMachineConfiguration,

    memory_manager: MemoryManager,
    call_frames: std.ArrayList(CallFrame),
    stack: std.ArrayList(Value),
    strings: StringsType,
    globals: GlobalsType,

    const StringsType = std.HashMap(*objects.StringObject, *objects.StringObject, objects.StringObject.InterningContext, HASH_MAP_LOAD);
    const GlobalsType = std.HashMap(*objects.StringObject, Value, objects.StringObject.MapContext, HASH_MAP_LOAD);

    const Self = @This();

    pub fn init(self: *Self, mem_conf: MemoryManagerConfiguration, vm_conf: VirtualMachineConfiguration) !void {
        self.conf = vm_conf;

        self.memory_manager = MemoryManager.init(mem_conf, self);
        const allocator = self.memory_manager.allocator();

        self.call_frames = std.ArrayList(CallFrame).init(allocator);
        try self.call_frames.ensureTotalCapacity(self.conf.call_frame_count);

        self.stack = std.ArrayList(Value).init(allocator);
        try self.stack.ensureTotalCapacity(self.conf.stack_size);

        self.strings = StringsType.init(allocator);

        self.globals = GlobalsType.init(allocator);
    }

    pub fn deinit(self: *Self) void {
        self.memory_manager.deinit();
        self.call_frames.deinit();
        self.stack.deinit();
        self.strings.deinit();
        self.globals.deinit();
    }

    pub fn getAllocator(self: *Self) std.mem.Allocator {
        return self.memory_manager.allocator();
    }

    pub fn run(self: *Self, chunk: *Chunk) !void {
        try self.call_frames.append(.{ .chunk = chunk, .ip = 0 });
        try self.runCycles();
    }

    fn runCycles(self: *Self) !void {
        while (true) {
            var frame = &self.call_frames.items[self.call_frames.items.len - 1];

            if (self.conf.trace_execution) {
                try self.traceStack();
                // NOTE: VM does not expect that the chunk could be empty.
                _ = try frame.chunk.disassembleInstruction(self.conf.debug_output, frame.ip);
            }

            const inst = try frame.readInstruction();
            switch (inst) {
                .Return => {
                    break;
                },

                .PushConstant => {
                    const constant = try frame.chunk.getConstant(inst.PushConstant.index);
                    try self.stackPush(constant);
                },

                .Negate => {
                    const value = (try self.stackPopType(.Integer)).Integer;
                    try self.stackPush(.{ .Integer = -value });
                },

                .Print => {
                    const value = try self.stackPop();
                    try value.print(self.conf.user_output);
                },

                .Add => {
                    try self.binaryOperation(.Add);
                },

                .Subtract => {
                    try self.binaryOperation(.Subtract);
                },

                .Multiply => {
                    try self.binaryOperation(.Multiply);
                },

                .Divide => {
                    try self.binaryOperation(.Divide);
                },

                .Greater => {
                    try self.binaryOperation(.Greater);
                },

                .Less => {
                    try self.binaryOperation(.Less);
                },

                .Pop => {
                    _ = try self.stackPop();
                },

                .Plus => {},

                .Not => {
                    const value = try self.stackPop();
                    try self.stackPush(.{ .Boolean = value.isFalse() });
                },

                .Equal => {
                    const b = try self.stackPop();
                    const a = try self.stackPop();

                    try self.stackPush(.{ .Boolean = a.isEqualTo(b) });
                },

                .JumpIfFalse => {
                    const val = try self.stackPeek();

                    if (val.isFalse()) {
                        frame.ip += inst.JumpIfFalse.jump;
                    }
                },

                .JumpIfTrue => {
                    const val = try self.stackPeek();

                    if (val.isTrue()) {
                        frame.ip += inst.JumpIfTrue.jump;
                    }
                },

                .PushTrue => {
                    try self.stackPush(.{ .Boolean = true });
                },

                .PushFalse => {
                    try self.stackPush(.{ .Boolean = true });
                },

                .PushNull => {
                    try self.stackPush(.{ .Null = {} });
                },

                .DefineGlobal => {
                    const val = try self.stackPop();

                    const str = try frame.chunk.getStringConstant(inst.DefineGlobal.index);

                    try self.globals.put(str, val);
                },

                .GetGlobal => {
                    const str = try frame.chunk.getStringConstant(inst.GetGlobal.index);

                    if (self.globals.get(str)) |val| {
                        try self.stackPush(val);
                    } else {
                        try std.io.getStdErr().writer().print("Runtime error: undefined reference to '{s}'.\n", .{str.str});
                        return RuntimeError.UndefinedReference;
                    }
                },

                .Unknown => {
                    try std.io.getStdErr().writer().print("Runtime error: unknown opcode 0x{x}.\n", .{inst.Unknown.opcode});
                    return RuntimeError.UnknownOpcode;
                },
            }
        }
    }

    const BinaryOperation = enum {
        Add,
        Subtract,
        Multiply,
        Divide,
        Greater,
        Less,
    };

    fn binaryOperation(self: *Self, op: BinaryOperation) !void {
        const b = (try self.stackPopType(.Integer)).Integer;
        const a = (try self.stackPopType(.Integer)).Integer;

        switch (op) {
            .Add => {
                const c = a + b;
                try self.stackPush(.{ .Integer = c });
            },

            .Subtract => {
                const c = a - b;
                try self.stackPush(.{ .Integer = c });
            },

            .Multiply => {
                const c = a * b;
                try self.stackPush(.{ .Integer = c });
            },

            .Divide => {
                if (b == 0) {
                    return RuntimeError.ZeroDivision;
                }

                const c = @divTrunc(a, b);
                try self.stackPush(.{ .Integer = c });
            },

            .Greater => {
                const c = a > b;
                try self.stackPush(.{ .Boolean = c });
            },

            .Less => {
                const c = a < b;
                try self.stackPush(.{ .Boolean = c });
            },
        }
    }

    fn stackPush(self: *Self, value: Value) !void {
        if (self.stack.items.len > self.conf.stack_size) {
            return RuntimeError.StackOverflow;
        }

        try self.stack.append(value);
    }

    fn stackPop(self: *Self) !Value {
        if (self.stack.items.len == 0) {
            return RuntimeError.StackUnderflow;
        }

        return self.stack.pop();
    }

    fn stackPopType(self: *Self, kind: ValueType) !Value {
        const val = try self.stackPop();

        if (val == kind) {
            return val;
        } else {
            try std.io.getStdErr().writer().print("Runtime error: wrong type, expected {s}, got {s}.\n", .{ @tagName(kind), @tagName(val) });
            return RuntimeError.WrongType;
        }
    }

    fn stackPopObject(self: *Self, kind: ObjectType, comptime T: type) !*T {
        const val = try self.stackPopType(.Object);

        if (val.Object.kind == kind) {
            return val.Object.as(T);
        } else {
            try std.io.getStdErr().writer().print("Runtime error: wrong type, expected {s}, got {s}.\n", .{ @tagName(kind), @tagName(val.Object.kind) });
            return RuntimeError.WrongType;
        }
    }

    fn stackPeek(self: *Self) !Value {
        if (self.stack.items.len == 0) {
            return RuntimeError.StackUnderflow;
        }

        return self.stack.getLast();
    }

    fn traceStack(self: *const Self) !void {
        var out = self.conf.debug_output;

        try out.writeAll("    ");

        for (self.stack.items) |value| {
            try out.writeAll("[ ");
            try value.print(out);
            try out.writeAll(" ]");
        }

        try out.writeAll("\n");
    }
};

pub const CallFrame = struct {
    chunk: *const Chunk,
    ip: usize,

    const Self = @This();

    pub fn readInstruction(self: *Self) !Instruction {
        const read_res = try self.chunk.readAt(self.ip);
        self.ip = read_res.offset;
        return read_res.inst;
    }
};
