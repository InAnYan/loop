#include "String.h"

#include "../HashTable.h"
#include "../MemoryManager.h"
#include "../VirtualMachine.h"

ObjectString* ObjectStringNew(VirtualMachine* vm, char* str, size_t length, size_t hash)
{
    ObjectString* interned = NULL;
    if (HashTableGetStringKey(&vm->strings, str, length, hash, &interned))
    {
        MemoryManagerFree(&vm->memory_manager, str, length + 1);
        return interned;
    }

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

void ObjectStringPrint(const ObjectString* self, FILE* out)
{
    fprintf(out, "%s", self->str);
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
