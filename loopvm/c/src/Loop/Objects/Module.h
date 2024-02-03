#ifndef LOOP_OBJECTS_MODULE_H
#define LOOP_OBJECTS_MODULE_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef enum ObjectModuleState {
    ObjectModuleState_ScriptNotExecuted,
    ObjectModuleState_ScriptRunning,
    ObjectModuleState_ScriptExecuted,
} ObjectModuleState;

typedef struct ObjectModule {
    Object obj;
    ObjectString *name;
    ObjectString *parent_dir;
    ObjectFunction *script;
    size_t globals_count;
    Value *globals;
    HashTable exports;
    ObjectModuleState state;
} ObjectModule;

ObjectModule *ObjectModuleNew(VirtualMachine *vm, ObjectString *name, ObjectString *parent_dir, size_t globals_count);

/// module can be NULL.
ObjectModule *ObjectModuleFromJSON(VirtualMachine *vm, ObjectString *path, const cJSON *data);

void ObjectModuleFree(ObjectModule *self, VirtualMachine *vm);

void ObjectModulePrint(const ObjectModule *self, FILE *out);

void ObjectModuleMarkTraverse(ObjectModule *self, MemoryManager *memory);

#endif // LOOP_OBJECTS_MODULE_H
