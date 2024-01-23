#include "Module.h"

#include "../VirtualMachine.h"

#include "Function.h"
#include "String.h"

ObjectModule* ObjectModuleNew(VirtualMachine* vm, ObjectString* name, ObjectString* path)
{
    ObjectModule* obj = ALLOCATE_OBJECT(vm, Module);

    ObjectFunction* script = ObjectFunctionNew(vm, obj, vm->common_strings.script, 0);

    obj->is_partial = true;
    obj->name = name;
    obj->path = path;
    obj->script = script;
    HashTableInit(&obj->exports);
    HashTableInit(&obj->globals);

    return obj;
}

ObjectModule* ObjectModuleFromJSON(VirtualMachine* vm, ObjectModule* _module, const cJSON* data)
{
    assert(cJSON_IsObject(data));

    const cJSON* name_json = cJSON_GetObjectItemCaseSensitive(data, "name");
    ObjectString* name = ObjectStringFromJSON(vm, NULL, name_json);

    const cJSON* path_json = cJSON_GetObjectItemCaseSensitive(data, "path");
    ObjectString* path = ObjectStringFromJSON(vm, NULL, path_json);

    ObjectModule* module = ObjectModuleNew(vm, name, path);
    bool put_res = HashTablePut(&vm->modules, vm, ValueObject((Object*)module->name), ValueObject((Object*)module));
    assert(put_res);

    const cJSON* chunk_json = cJSON_GetObjectItemCaseSensitive(data, "chunk");
    ChunkFromJSON(&module->script->chunk, vm, module, chunk_json);

    return module;
}

void ObjectModuleFree(ObjectModule* self, VirtualMachine* vm)
{
    self->name = NULL;
    self->path = NULL;
    self->script = NULL;
    HashTableDeinit(&self->exports, vm);
    HashTableDeinit(&self->globals, vm);
    FREE_OBJECT(vm, self, Module);
}

void ObjectModulePrint(const ObjectModule* self, FILE* out)
{
    fprintf(out, "<module at 0x%p>", self);
}
