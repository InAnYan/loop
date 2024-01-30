const std = @import("std");
const RuntimeError = @import("errors.zig").RuntimeError;
const objects = @import("objects.zig");
const VirtualMachine = @import("virtual_machine.zig").VirtualMachine;

pub const ValueType = enum {
    Null,
    Integer,
    Boolean,
    Object,
};

pub const Value = union(ValueType) {
    Null: void,
    Integer: isize,
    Boolean: bool,
    Object: *objects.Object,

    const Self = @This();

    pub fn fromJSON(vm: *VirtualMachine, value: std.json.Value) !Self {
        const t = value.object.get("type").?.string;
        if (std.mem.eql(u8, t, "Integer")) {
            const n = value.object.get("data").?.integer;
            return .{ .Integer = n };
        } else if (std.mem.eql(u8, t, "String")) {
            const json_str = value.object.get("data").?.string;

            const buf = try vm.getAllocator().alloc(u8, json_str.len);
            std.mem.copyForwards(u8, buf, json_str);

            const s = try objects.newString(vm, buf);

            return .{ .Object = &s.obj };
        } else {
            return RuntimeError.WrongJSON;
        }
    }

    pub fn print(self: *const Self, writer: anytype) !void {
        return switch (self.*) {
            .Null => writer.writeAll("null"),

            .Integer => |n| writer.print("{d}", .{n}),

            .Boolean => |b| if (b) {
                return writer.writeAll("true");
            } else {
                return writer.writeAll("false");
            },

            .Object => |r| r.print(writer),
        };
    }

    pub fn isEqualTo(self: Self, other: Self) bool {
        if (@intFromEnum(self) != @intFromEnum(other)) {
            return false;
        }

        return switch (self) {
            .Null => true,
            .Integer => |n| n == other.Integer,
            .Boolean => |b| b == other.Boolean,
            .Object => |r| r == other.Object,
        };
    }

    pub fn isTrue(self: *const Self) bool {
        return !self.isFalse();
    }

    pub fn isFalse(self: *const Self) bool {
        return switch (self.*) {
            .Null => true,
            .Integer => false,
            .Boolean => |b| !b,
            .Object => false,
        };
    }
};
