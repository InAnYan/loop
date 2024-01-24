#ifndef LOOP_OBJECTS_MODULE_H
#define LOOP_OBJECTS_MODULE_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef struct ObjectModule
{
    Object obj;
    ObjectString* name;
    ObjectString* parent_dir;
    ObjectFunction* script;
    HashTable exports;
    HashTable globals;
    bool is_partial;
} ObjectModule;

ObjectModule* ObjectModuleNew(VirtualMachine* vm, ObjectString* name, ObjectString* parent_dir);
/// module can be NULL.
ObjectModule* ObjectModuleFromJSON(VirtualMachine* vm, ObjectString* path, const cJSON* data);
void ObjectModuleFree(ObjectModule* self, VirtualMachine* vm);

void ObjectModulePrint(const ObjectModule* self, FILE* out);

#endif // LOOP_OBJECTS_MODULE_H
