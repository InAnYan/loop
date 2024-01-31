#ifndef LOOP_OBJECTS_CLASS_H
#define LOOP_OBJECTS_CLASS_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef struct ObjectClass
{
    Object obj;
    ObjectModule* module; // It's used only for printing.
    ObjectString* name;
    HashTable methods;
} ObjectClass;

ObjectClass* ObjectClassNew(VirtualMachine* vm, ObjectModule* module, ObjectString* name);
ObjectClass* ObjectClassFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectClassFree(ObjectClass* self, VirtualMachine* vm);

void ObjectClassPrint(const ObjectClass* self, FILE* out);

void ObjectClassMarkTraverse(ObjectClass* self, MemoryManager* memory);

#endif // LOOP_OBJECTS_CLASS_H
