#include "Object.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"

#include "Objects/Dictionary.h"
#include "Objects/Function.h"
#include "Objects/Module.h"
#include "Objects/String.h"

Object* ObjectAllocateRaw(VirtualMachine* vm, ObjectType type, size_t size)
{
    Object* obj = MemoryManagerReallocate(&vm->memory_manager, NULL, size, 0);
    obj->marked = false;
    obj->type = type;
    obj->next = vm->memory_manager.objects;
    vm->memory_manager.objects = obj;
    return obj;
}

void ObjectFreeRaw(VirtualMachine* vm, Object* obj, size_t size)
{
    obj->next = NULL;
    obj->marked = false;
    MemoryManagerFree(&vm->memory_manager, obj, size);
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

void ObjectPrint(const Object* self, FILE* out)
{
    switch (ObjectGetType(self))
    {
        #define OBJECT_PRINT(name) \
                case ObjectType_##name: \
                    Object##name##Print(ObjectAs##name##Const(self), out); \
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
