#ifndef LOOP_OBJECTS_UPVALUE_H
#define LOOP_OBJECTS_UPVALUE_H

#include "../Common.h"
#include "../Object.h"

#include "../Value.h"

typedef struct ObjectUpvalue {
    Object obj;
    Value *location;
    Value closed;
    ObjectUpvalue *next;
} ObjectUpvalue;

ObjectUpvalue *ObjectUpvalueNew(VirtualMachine *vm, Value *location, ObjectUpvalue *next);

void ObjectUpvalueFree(ObjectUpvalue *self, VirtualMachine *vm);

void ObjectUpvaluePrint(const ObjectUpvalue *self, FILE *out);

void ObjectUpvalueMarkTraverse(ObjectUpvalue *self, MemoryManager *memory);

#endif // LOOP_OBJECTS_UPVALUE_H
