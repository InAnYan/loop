#include "Module.h"

#include "../VirtualMachine.h"

#include "Function.h"
#include "String.h"

ObjectModule* ObjectModuleNew(VirtualMachine* vm, ObjectString* name, ObjectString* parent_dir)
{
    assert(name != NULL);
    assert(parent_dir != NULL);

    ObjectModule* obj = ALLOCATE_OBJECT(vm, Module);

    ObjectFunction* script = ObjectFunctionNew(vm, obj, vm->common.script, 0);

    obj->is_partial = true;
    obj->name = name;
    obj->parent_dir = parent_dir;
    obj->script = script;
    HashTableInit(&obj->exports);
    HashTableInit(&obj->globals);

    return obj;
}

ObjectModule* ObjectModuleFromJSON(VirtualMachine* vm, ObjectString* path, const cJSON* data)
{
    assert(cJSON_IsObject(data));

    // TODO: Non portable path split. And probably ugly algorithm.
    int dot = -1;
    int slash = -1;

    for (int i = 0; i < path->length; i++)
    {
        if (path->str[i] == '.')
        {
            dot = i;
        }

        if (path->str[i] == '/' || path->str[i] == '\\')
        {
            slash = i;
        }
    }

    ObjectString* name = NULL;
    ObjectString* parent_dir = NULL;

    // TODO: Bug if slash at the end.
    name = ObjectStringSubstring(vm, path, (slash != -1 ? slash + 1 : 0), (dot != -1 ? dot : path->length - 1));
    parent_dir = ObjectStringSubstring(vm, path, 0, (slash != -1 ? slash : 0));

    ObjectModule* module = ObjectModuleNew(vm, name, parent_dir);
    bool put_res = HashTablePut(&vm->modules, vm, ValueObject((Object*)module->name), ValueObject((Object*)module));
    assert(put_res);

    ChunkFromJSON(&module->script->chunk, vm, module, data);

    return module;
}

void ObjectModuleFree(ObjectModule* self, VirtualMachine* vm)
{
    self->name = NULL;
    self->parent_dir = NULL;
    self->script = NULL;
    HashTableDeinit(&self->exports, vm);
    HashTableDeinit(&self->globals, vm);
    FREE_OBJECT(vm, self, Module);
}

void ObjectModulePrint(const ObjectModule* self, FILE* out)
{
    fprintf(out, "<module %s>", self->name->str);
}
