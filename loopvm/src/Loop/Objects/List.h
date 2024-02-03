#ifndef LOOP_OBJECTS_LIST_H
#define LOOP_OBJECTS_LIST_H

#include "../Common.h"
#include "../Object.h"

typedef struct ObjectList {
    Object obj;
    size_t count;
    size_t capacity;
    Value *elements;
} ObjectList;

ObjectList *ObjectListNew(VirtualMachine *vm);

void ObjectListFree(ObjectList *self, VirtualMachine *vm);

void ObjectListPush(ObjectList *self, VirtualMachine *vm, Value value);

void ObjectListPrint(const ObjectList *self, FILE *out);

void ObjectListMarkTraverse(ObjectList *self, MemoryManager *memory);

#endif // LOOP_OBJECTS_LIST_H
