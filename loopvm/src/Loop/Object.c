#include "Object.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"

const char* ObjectTypeToString(ObjectType self)
{
    switch (self)
    {
        #define ObjectType_TO_STRING(name) case ObjectType_##name: return #name;

    ObjectType_LIST(ObjectType_TO_STRING);

        #undef ObjectType_TO_STRING

    default:
        return "UnknownObjectType";
    }
}

Object* ObjectFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* json)
{
    assert(cJSON_IsObject(json));

    const cJSON* type = cJSON_GetObjectItemCaseSensitive(json, "type");
    const cJSON* data = cJSON_GetObjectItemCaseSensitive(json, "data");

    assert(cJSON_IsString(type));
    const char* type_string = type->valuestring;

    #define OBJECT_FROM_JSON(name) \
    if (strcmp(type_string, #name) == 0) \
    { \
        return (Object*)Object##name##FromJSON(vm, module, data); \
    }

    ObjectType_LIST(OBJECT_FROM_JSON);

    #undef OBJECT_FROM_JSON

    assert(false && "Invalid JSON");
}

ObjectType ObjectGetType(const Object* self)
{
    return self->type;
}

#define OBJECT_IS_IMPL(name) \
    bool ObjectIs##name(const Object* self) \
    { \
        return ObjectGetType(self) == ObjectType_##name; \
    }

ObjectType_LIST(OBJECT_IS_IMPL)

#undef OBJECT_IS_IMPL

#define OBJECT_AS_IMPL(name) \
    Object##name* ObjectAs##name(Object* self) \
    { \
        assert(ObjectIs##name(self)); \
        return (Object##name*)self; \
    }

ObjectType_LIST(OBJECT_AS_IMPL)

#undef OBJECT_AS_IMPL

#define OBJECT_AS_CONST_IMPL(name) \
    const Object##name* ObjectAs##name##Const(const Object* self) \
    { \
        assert(ObjectIs##name(self)); \
        return (const Object##name*)self; \
    }

ObjectType_LIST(OBJECT_AS_CONST_IMPL)

#undef OBJECT_AS_CONST_IMPL

void ObjectPrint(const Object* self, FILE* out, PrintFlags flags)
{
    switch (ObjectGetType(self))
    {
        #define OBJECT_PRINT(name) \
                case ObjectType_##name: \
                    Object##name##Print(ObjectAs##name##Const(self), out, flags); \
                    break;

    ObjectType_LIST(OBJECT_PRINT)

        #undef OBJECT_PRINT
    default:
        assert(false && "Unimplemented object printing");
    }
}

void ObjectFree(Object* self, VirtualMachine* vm)
{
    switch (ObjectGetType(self))
    {
        #define OBJECT_FREE(name) \
                case ObjectType_##name: \
                    Object##name##Free(ObjectAs##name(self), vm); \
                    break;

    ObjectType_LIST(OBJECT_FREE)

        #undef OBJECT_FREE
    }
}

static Object* AllocateObjectRaw(VirtualMachine* vm, ObjectType type, size_t size)
{
    Object* obj = MemoryManagerReallocate(&vm->memory_manager, NULL, size, 0);
    obj->marked = false;
    obj->type = type;
    obj->next = vm->memory_manager.objects;
    vm->memory_manager.objects = obj;
    return obj;
}

#define ALLOCATE_OBJECT(vm, name) (Object##name*)AllocateObjectRaw(vm, ObjectType_##name, sizeof(Object##name))
#define FREE_OBJECT(vm, self, name) do \
    { \
        self->obj.next = NULL; \
        self->obj.marked = false; \
        MemoryManagerFree(&vm->memory_manager, self, sizeof(Object##name)); \
    } while (false)

ObjectModule* ObjectModuleNew(VirtualMachine* vm, ObjectString* name, ObjectString* path)
{
    ObjectModule* obj = ALLOCATE_OBJECT(vm, Module);

    ObjectFunction* script = ObjectFunctionNew(vm, obj, vm->common_strings.script, 0);

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

    const cJSON* chunk_json = cJSON_GetObjectItemCaseSensitive(data, "chunk");
    ChunkFromJSON(&module->script->chunk, vm, module, chunk_json);

    return module;
}

void ObjectModuleFree(ObjectModule* self, VirtualMachine* vm)
{
    ObjectStringFree(self->name, vm);
    ObjectStringFree(self->path, vm);
    ObjectFunctionFree(self->script, vm);
    HashTableDeinit(&self->exports, vm);
    HashTableDeinit(&self->globals, vm);
}

void ObjectModulePrint(const ObjectModule* self, FILE* out, PrintFlags flags)
{
    fprintf(out, "<module at 0x%p>", self);
}

ObjectString* ObjectStringNew(VirtualMachine* vm, const char* str, size_t length, size_t hash)
{
    ObjectString* interned = NULL;
    printf("INTERNING: %s\n", str);
    if (HashTableGetStringKey(&vm->strings, str, length, hash, &interned))
    {
        MemoryManagerFree(&vm->memory_manager, str, length + 1);
        printf("-- INTERNED\n");
        return interned;
    }

    printf("-- NEW\n");

    ObjectString* obj = ALLOCATE_OBJECT(vm, String);
    obj->str = str;
    obj->length = length;
    obj->hash = hash;

    Object* bare = (Object*)obj;
    HashTablePut(&vm->strings, vm, ValueObject(bare), ValueObject(bare));

    return obj;
}

ObjectString* ObjectStringFromLiteral(VirtualMachine* vm, const char* str)
{
    size_t length = strlen(str);
    char* new_str = MemoryManagerAllocate(&vm->memory_manager, length + 1);
    strcpy(new_str, str);

    return ObjectStringNew(vm, new_str, length, CalculateStringHash(str, length));
}

ObjectString* ObjectStringFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data)
{
    assert(cJSON_IsString(data));

    const size_t length = strlen(data->valuestring);
    const size_t hash = CalculateStringHash(data->valuestring, length);

    char* new_str = MemoryManagerAllocate(&vm->memory_manager, length + 1);
    strcpy(new_str, data->valuestring);

    return ObjectStringNew(vm, new_str, length, hash);
}

void ObjectStringFree(ObjectString* self, VirtualMachine* vm)
{
    MemoryManagerFree(&vm->memory_manager, self->str, self->length + 1);
    self->hash = 0;
    self->length = 0;
    self->str = NULL;
    FREE_OBJECT(vm, self, String);
}

void ObjectStringPrint(const ObjectString* self, FILE* out, PrintFlags flags)
{
    switch (flags)
    {
    case PrintFlags_Debug:
        // TODO: ObjectStringPrint with PrintFlags_debug escaped characters.
        fprintf(out, "\"%s\"", self->str);
        break;
    case PrintFlags_Pretty:
        fprintf(out, "%s", self->str);
        break;
    }
}

size_t CalculateStringHash(const char* str, size_t length)
{
    // TODO: That's not right.

    uint32_t hash = 2166136261u;

    for (int i = 0; i < length; i++)
    {
        hash ^= str[i];
        hash *= 16777619;
    }

    return hash;
}

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

void ObjectFunctionPrint(const ObjectFunction* self, FILE* out, PrintFlags flags)
{
    fprintf(out, "<function %s.%s>", self->module->name->str, self->name->str);
}
