const VirtualMachine = @import("virtual_machine.zig").VirtualMachine;
const Allocator = @import("std").mem.Allocator;
const std = @import("std");

pub const ObjectType = enum { String };

pub const Object = struct {
    kind: ObjectType,
    next: ?*Object,

    const Self = @This();

    pub fn destroy(self: *Self, allocator: Allocator) void {
        switch (self.kind) {
            .String => {
                const str = self.as(StringObject);
                allocator.free(str.str);
                allocator.destroy(str);
            },
        }
    }

    pub fn print(self: *Self, writer: anytype) !void {
        switch (self.kind) {
            .String => {
                const str = self.as(StringObject);
                try str.print(writer);
            },
        }
    }

    pub fn as(self: *Self, comptime T: type) *T {
        if (self.kind == T.getType()) {
            return @as(*T, @ptrCast(self));
        } else {
            @panic("wrong object 'as'");
        }
    }
};

pub const StringObject = struct {
    obj: Object,
    str: []const u8,
    hash: usize,

    pub const InterningContext = struct {
        const Self = @This();

        pub fn hash(self: Self, str: *StringObject) u64 {
            _ = self;
            return str.hash;
        }

        pub fn eql(self: Self, a: *StringObject, b: *StringObject) bool {
            _ = self;
            return std.mem.eql(u8, a.str, b.str);
        }
    };

    pub const MapContext = struct {
        const Self = @This();

        pub fn hash(self: Self, str: *StringObject) u64 {
            _ = self;
            return str.hash;
        }

        pub fn eql(self: Self, a: *StringObject, b: *StringObject) bool {
            _ = self;
            return @intFromPtr(a) == @intFromPtr(b);
        }
    };

    pub fn print(self: *StringObject, writer: anytype) !void {
        return writer.writeAll(self.str);
    }

    pub fn getType() ObjectType {
        return .String;
    }
};

fn allocateObject(vm: *VirtualMachine, comptime T: type) !*T {
    var obj = try vm.getAllocator().create(T);

    obj.obj.kind = T.getType();

    obj.obj.next = vm.memory_manager.objects;
    vm.memory_manager.objects = &obj.obj;

    return obj;
}

pub fn newString(vm: *VirtualMachine, str: []const u8) !*StringObject {
    // Usually, I write two functions: newStringTake (takes owned pointer) and newStringIntern (takes a view (string_view) or slice).
    // But because there is no parser, I don't really need newStringIntern.

    // Well, not really good idea.

    var obj = try allocateObject(vm, StringObject);
    obj.str = str;
    obj.hash = std.hash.Fnv1a_64.hash(str);

    if (vm.strings.get(obj)) |interned| {
        vm.memory_manager.objects = obj.obj.next;
        obj.obj.destroy(vm.getAllocator());
        return interned;
    }

    try vm.strings.put(obj, obj);
    return obj;
}
