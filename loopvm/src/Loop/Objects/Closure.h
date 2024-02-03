#ifndef LOOP_OBJECTS_CLOSURE_H
#define LOOP_OBJECTS_CLOSURE_H

#include "../Common.h"
#include "../Object.h"

typedef struct ObjectClosure {
    Object obj;
    ObjectFunction *function;
    size_t upvalue_count;
    ObjectUpvalue **upvalues;
} ObjectClosure;

ObjectClosure *ObjectClosureNew(VirtualMachine *vm, ObjectFunction *function, size_t upvalue_count);

void ObjectClosureFree(ObjectClosure *self, VirtualMachine *vm);

void ObjectClosurePrint(const ObjectClosure *self, FILE *out);

void ObjectClosureMarkTraverse(ObjectClosure *self, MemoryManager *memory);

#endif // LOOP_OBJECTS_CLOSURE_H
