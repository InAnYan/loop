const std = @import("std");
const RuntimeError = @import("errors.zig").RuntimeError;
const objects = @import("objects.zig");
const VirtualMachine = @import("virtual_machine.zig").VirtualMachine;

pub const Value = union(enum) {
    integer: isize,
    boolean: bool,
    ref: *objects.Object,

    const Self = @This();

    pub fn fromJSON(vm: *VirtualMachine, value: std.json.Value) !Self {
        const t = value.object.get("type").?.string;
        if (std.mem.eql(u8, t, "integer")) {
            const n = value.object.get("data").?.integer;
            return .{ .integer = n };
        } else if (std.mem.eql(u8, t, "bool")) {
            const b = value.object.get("data").?.bool;
            return .{ .boolean = b };
        } else if (std.mem.eql(u8, t, "string")) {
            const json_str = value.object.get("data").?.string;

            const buf = try vm.getAllocator().alloc(u8, json_str.len - 2);
            std.mem.copyForwards(u8, buf, json_str[1 .. json_str.len - 1]);

            const s = try objects.newString(vm, buf);

            return .{ .ref = &s.obj };
        } else {
            return RuntimeError.WrongJSON;
        }
    }

    pub fn print(self: *const Self, writer: anytype) !void {
        return switch (self.*) {
            .integer => |n| writer.print("{d}", .{n}),

            .boolean => |b| if (b) {
                return writer.writeAll("true");
            } else {
                return writer.writeAll("false");
            },

            .ref => |r| r.print(writer),
        };
    }

    pub fn isEqualTo(self: Self, other: Self) bool {
        if (@intFromEnum(self) != @intFromEnum(other)) {
            return false;
        }

        return switch (self) {
            .integer => |n| n == other.integer,
            .boolean => |b| b == other.boolean,
            .ref => |r| r == other.ref,
        };
    }

    pub fn isTrue(self: *const Self) bool {
        return !self.isFalse();
    }

    pub fn isFalse(self: *const Self) bool {
        return switch (self.*) {
            .integer => false,
            .ref => false,
            .boolean => |b| !b,
        };
    }
};
