#include "Upvalue.h"

ObjectUpvalue *ObjectUpvalueNew(VirtualMachine *vm, Value *location, ObjectUpvalue *next) {
    ObjectUpvalue *obj = ALLOCATE_OBJECT(vm, Upvalue);
    obj->location = location;
    obj->closed = ValueNull();
    obj->next = next;
    return obj;
}

void ObjectUpvalueFree(ObjectUpvalue *self, VirtualMachine *vm) {
    self->location = NULL;
    self->next = NULL;
    self->closed = ValueNull();
    FREE_OBJECT(vm, self, Upvalue);
}

void ObjectUpvaluePrint(const ObjectUpvalue *self, FILE *out) {
    fprintf(out, "<upvalue>");
}

void ObjectUpvalueMarkTraverse(ObjectUpvalue *self, MemoryManager *memory) {
    ValueMark(self->closed, memory);
}
