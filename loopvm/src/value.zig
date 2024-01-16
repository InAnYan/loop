const std = @import("std");
const RuntimeError = @import("errors.zig").RuntimeError;

pub const Value = union(enum) {
    integer: isize,
    boolean: bool,

    const Self = @This();

    pub fn fromJSON(value: std.json.Value) !Self {
        const t = value.object.get("type").?.string;
        if (std.mem.eql(u8, t, "integer")) {
            const n = value.object.get("data").?.integer;
            return Self{ .integer = n };
        } else if (std.mem.eql(u8, t, "bool")) {
            const b = value.object.get("data").?.bool;
            return Self{ .boolean = b };
        } else {
            return RuntimeError.WrongJSON;
        }
    }

    pub fn print(self: *const Self, writer: anytype) !void {
        switch (self.*) {
            .integer => |n| try writer.print("{d}", .{n}),
            .boolean => |b| if (b) {
                try writer.writeAll("true");
            } else {
                try writer.writeAll("false");
            },
        }
    }

    pub fn isEqualTo(self: Self, other: Self) bool {
        if (@intFromEnum(self) != @intFromEnum(other)) {
            return false;
        }

        return switch (self) {
            .integer => |n| n == other.integer,
            .boolean => |b| b == other.boolean,
        };
    }

    pub fn isTrue(self: *const Self) bool {
        return !self.isFalse();
    }

    pub fn isFalse(self: *const Self) bool {
        return switch (self.*) {
            .integer => false,
            .boolean => |b| b,
        };
    }
};
