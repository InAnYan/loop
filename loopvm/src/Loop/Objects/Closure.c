#include "Closure.h"

#include "../MemoryManager.h"
#include "../VirtualMachine.h"

#include "Function.h"
#include "Module.h"
#include "String.h"

ObjectClosure *ObjectClosureNew(VirtualMachine *vm, ObjectFunction *function, size_t upvalue_count) {
    ObjectUpvalue **upvalues = ALLOC_ARRAY(vm, ObjectUpvalue*, upvalue_count);
    for (size_t i = 0; i < upvalue_count; i++) {
        upvalues[i] = NULL;
    }

    ObjectClosure *obj = ALLOCATE_OBJECT(vm, Closure);
    obj->function = function;
    obj->upvalue_count = upvalue_count;
    obj->upvalues = upvalues;
    return obj;
}

void ObjectClosureFree(ObjectClosure *self, VirtualMachine *vm) {
    self->function = NULL;
    FREE_ARRAY(vm, self->upvalues, ObjectUpvalue*, self->upvalue_count);
    self->upvalue_count = 0;
    self->upvalues = NULL;
    FREE_OBJECT(vm, self, Closure);
}

void ObjectClosurePrint(const ObjectClosure *self, FILE *out) {
    fprintf(out, "<closure %s.%s>", self->function->module->name->str, self->function->name->str);
}

void ObjectClosureMarkTraverse(ObjectClosure *self, MemoryManager *memory) {
    ObjectMark((Object *) self->function, memory);
    for (size_t i = 0; i < self->upvalue_count; i++) {
        ObjectMarkMaybeNull((Object *) self->upvalues[i], memory);
    }
}
