#ifndef LOOP_OBJECTS_MODULE_H
#define LOOP_OBJECTS_MODULE_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef struct ObjectModule
{
    Object obj;
    ObjectString* name;
    ObjectString* path;
    ObjectFunction* script;
    HashTable exports;
    HashTable globals;
    bool is_partial;
} ObjectModule;

ObjectModule* ObjectModuleNew(VirtualMachine* vm, ObjectString* name, ObjectString* path);
/// module can be NULL.
ObjectModule* ObjectModuleFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectModuleFree(ObjectModule* self, VirtualMachine* vm);

void ObjectModulePrint(const ObjectModule* self, FILE* out);

#endif // LOOP_OBJECTS_MODULE_H
