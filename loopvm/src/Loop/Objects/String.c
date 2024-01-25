#include "String.h"

#include "../HashTable.h"
#include "../MemoryManager.h"
#include "../VirtualMachine.h"

ObjectString* ObjectStringNew(VirtualMachine* vm, char* str, size_t length, size_t hash)
{
    ObjectString* interned = NULL;
    if (HashTableGetStringKey(&vm->strings, str, length, hash, &interned))
    {
        FREE_ARRAY(vm, str, char, length + 1);
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
    char* new_str = ALLOC_ARRAY(vm, char, length + 1);
    strcpy(new_str, str);

    return ObjectStringNew(vm, new_str, length, CalculateStringHash(str, length));
}

ObjectString* ObjectStringFromJSON(VirtualMachine* vm, const cJSON* data)
{
    assert(cJSON_IsString(data));

    const size_t length = strlen(data->valuestring);
    const size_t hash = CalculateStringHash(data->valuestring, length);

    char* new_str = ALLOC_ARRAY(vm, char, length + 1);
    strcpy(new_str, data->valuestring);

    return ObjectStringNew(vm, new_str, length, hash);
}

void ObjectStringFree(ObjectString* self, VirtualMachine* vm)
{
    FREE_ARRAY(vm, self->str, char, self->length + 1);
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

ObjectString* ObjectStringConcatenate(VirtualMachine* vm, const ObjectString* left, const ObjectString* right)
{
    const size_t length = left->length + right->length;
    char* str = ALLOC_ARRAY(vm, char, length + 1);
    strcpy(str, left->str);
    strcpy(str + left->length, right->str);
    return ObjectStringNew(vm, str, length, left->hash);
}

ObjectString* ObjectStringSubstring(VirtualMachine* vm, ObjectString* str, size_t start, size_t end)
{
    // This checks are used in module loading.

    if (start == end)
    {
        return vm->common.empty_string;
    }

    if (start == 0 && end == str->length - 1)
    {
        return str;
    }

    const size_t length = end - start;
    char* str_sub = ALLOC_ARRAY(vm, char, length + 1);
    strncpy(str_sub, str->str + start, length);
    str_sub[length] = '\0';
    return ObjectStringNew(vm, str_sub, length, CalculateStringHash(str_sub, length));
}

void ObjectStringMarkTraverse(ObjectString* self, MemoryManager* memory)
{

}
