#ifndef LOOP_OBJECTS_CLASS_H
#define LOOP_OBJECTS_CLASS_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef struct ObjectClass
{
    Object obj;
    ObjectModule* module; // It's used only for printing.
    ObjectClass* super;
    ObjectString* name;
    HashTable methods;
} ObjectClass;

ObjectClass* ObjectClassNew(VirtualMachine* vm, ObjectModule* module, ObjectClass* super, ObjectString* name);
ObjectClass* ObjectClassFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectClassFree(ObjectClass* self, VirtualMachine* vm);

void ObjectClassPrint(const ObjectClass* self, FILE* out);

#endif // LOOP_OBJECTS_CLASS_H
