#include "Object.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"

#include "Objects/Dictionary.h"
#include "Objects/Function.h"
#include "Objects/Module.h"
#include "Objects/String.h"
#include "Objects/Class.h"
#include "Objects/Instance.h"
#include "Objects/BoundMethod.h"

Object* ObjectAllocateRaw(VirtualMachine* vm, ObjectType type, size_t size)
{
    Object* obj = MemoryManagerReallocate(&vm->memory_manager, NULL, size, 0);
    obj->marked = false;
    obj->type = type;
    obj->next = vm->memory_manager.objects;
    vm->memory_manager.objects = obj;

    #ifdef LOG_GC
    fprintf(DEBUG_OUT, "-- Alloc: %zu bytes of %s - %p\n",
            size, ObjectTypeToString(type), obj);
    #endif

    return obj;
}

void ObjectFreeRaw(VirtualMachine* vm, Object* obj, size_t size)
{
    #ifdef LOG_GC
    fprintf(DEBUG_OUT, "-- Free: %s - %p\n",
            ObjectTypeToString(obj->type), obj);
    #endif

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

    if (strcmp(type_string, "String") == 0)
    {
        return (Object*)ObjectStringFromJSON(vm, data);
    }

    if (strcmp(type_string, "Function") == 0)
    {
        return (Object*)ObjectFunctionFromJSON(vm, module, data);
    }

    if (strcmp(type_string, "Class") == 0)
    {
        return (Object*)ObjectClassFromJSON(vm, module, data);
    }

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

void ObjectMark(Object* self, MemoryManager* memory)
{
    assert(self != NULL);

    if (self->marked)
    {
        return;
    }

    #ifdef LOG_GC
    fprintf(DEBUG_OUT, "-- Mark: %p - ", self);
    ObjectPrint(self, DEBUG_OUT);
    fprintf(DEBUG_OUT, "\n");
    #endif

    self->marked = true;

    // Not really cool if Object is responsible for adding
    // itself to the working list of the Memory.

    if (memory->gray_stack_count + 1 > memory->gray_stack_capacity)
    {
        memory->gray_stack_capacity = GROW_CAPACITY(memory->gray_stack_capacity);
        memory->gray_stack = (Object**)realloc(memory->gray_stack, sizeof(Object*) * memory->gray_stack_capacity);

        if (memory->gray_stack == NULL)
        {
            fprintf(stderr, "FATAL ERROR: out of memory\n");
            exit(1);
        }
    }

    memory->gray_stack[memory->gray_stack_count++] = self;
}

void ObjectMarkTraverse(Object* self, MemoryManager* memory)
{
    assert(self != NULL);

    switch (ObjectGetType(self))
    {
        #define OBJECT_MARK_TRAVERSE(name) \
                case ObjectType_##name: \
                    Object##name##MarkTraverse(ObjectAs##name(self), memory); \
                    break;

    ObjectType_LIST(OBJECT_MARK_TRAVERSE)

        #undef OBJECT_MARK_TRAVERSE
    }
}
