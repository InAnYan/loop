#include "Class.h"

#include "String.h"
#include "Function.h"
#include "Module.h"

ObjectClass* ObjectClassNew(VirtualMachine* vm, ObjectModule* module, ObjectString* name)
{
    ObjectClass* obj = ALLOCATE_OBJECT(vm, Class);

    obj->module = module;
    obj->name = name;
    HashTableInit(&obj->methods);

    return obj;
}

ObjectClass* ObjectClassFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data)
{
    assert(cJSON_IsObject(data));
    assert(module != NULL);

    const cJSON* name_json = cJSON_GetObjectItemCaseSensitive(data, "name");
    ObjectString* name = ObjectStringFromJSON(vm, name_json);

    // The super class is set at runtime.
    ObjectClass* obj = ObjectClassNew(vm, module, name);

    const cJSON* methods_json = cJSON_GetObjectItemCaseSensitive(data, "methods");
    assert(cJSON_IsArray(methods_json));

    const cJSON* method_json = NULL;
    cJSON_ArrayForEach(method_json, methods_json)
    {
        const cJSON* method_func_json = cJSON_GetObjectItemCaseSensitive(method_json, "data");

        ObjectFunction* method = ObjectFunctionFromJSON(vm, module, method_func_json);
        bool res = HashTablePut(&obj->methods, vm,
                                ValueObject((Object*)method->name), ValueObject((Object*)method));
        assert(res);
    }

    return obj;
}

void ObjectClassFree(ObjectClass* self, VirtualMachine* vm)
{
    self->name = NULL;
    HashTableDeinit(&self->methods, vm);
    FREE_OBJECT(vm, self, Class);
}

void ObjectClassPrint(const ObjectClass* self, FILE* out)
{
    fprintf(out, "<class %s.%s>",
            self->module->name->str, self->name->str);
}

void ObjectClassMarkTraverse(ObjectClass* self, MemoryManager* memory)
{
    ObjectMark((Object*)self->module, memory);
    ObjectMark((Object*)self->name, memory);
    HashTableMark(&self->methods, memory);
}
