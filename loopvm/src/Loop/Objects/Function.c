#include "Function.h"

#include "String.h"
#include "Module.h"

ObjectFunction* ObjectFunctionNew(VirtualMachine* vm, ObjectModule* module, ObjectString* name, size_t arity)
{
    ObjectFunction* obj = ALLOCATE_OBJECT(vm, Function);
    obj->module = module;
    obj->name = name;
    obj->arity = arity;
    ChunkInit(&obj->chunk);
    return obj;
}

ObjectFunction* ObjectFunctionFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data)
{
    assert(cJSON_IsObject(data));

    const cJSON* name_json = cJSON_GetObjectItemCaseSensitive(data, "name");
    ObjectString* name = ObjectStringFromJSON(vm, module, name_json);

    const cJSON* arity_json = cJSON_GetObjectItemCaseSensitive(data, "arity");
    assert(cJSON_IsNumber(arity_json));
    const uint8_t arity = arity_json->valueint;

    const cJSON* chunk_json = cJSON_GetObjectItemCaseSensitive(data, "chunk");

    ObjectFunction* obj = ObjectFunctionNew(vm, module, name, arity);
    ChunkFromJSON(&obj->chunk, vm, module, chunk_json);

    return obj;
}

void ObjectFunctionFree(ObjectFunction* self, VirtualMachine* vm)
{
    self->module = NULL;
    self->name = NULL;
    self->arity = 0;
    ChunkDeinit(&self->chunk, vm);
    FREE_OBJECT(vm, self, Function);
}

void ObjectFunctionPrint(const ObjectFunction* self, FILE* out)
{
    fprintf(out, "<function %s.%s>", self->module->name->str, self->name->str);
}
