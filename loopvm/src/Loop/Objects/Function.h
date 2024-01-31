#ifndef LOOP_OBJECTS_FUNCTION_H
#define LOOP_OBJECTS_FUNCTION_H

#include "../Common.h"
#include "../Object.h"

#include "../Chunk.h"

typedef struct ObjectFunction
{
    Object obj;
    ObjectModule* module;
    ObjectString* name;
    size_t arity;
    Chunk chunk;
} ObjectFunction;

ObjectFunction* ObjectFunctionNew(VirtualMachine* vm, ObjectModule* module, ObjectString* name, size_t arity);
ObjectFunction* ObjectFunctionFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectFunctionFree(ObjectFunction* self, VirtualMachine* vm);

void ObjectFunctionPrint(const ObjectFunction* self, FILE* out);

void ObjectFunctionMarkTraverse(ObjectFunction* self, MemoryManager* memory);

#endif // LOOP_OBJECTS_FUNCTION_H
