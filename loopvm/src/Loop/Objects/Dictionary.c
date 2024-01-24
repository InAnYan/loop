#include "Dictionary.h"

ObjectDictionary* ObjectDictionaryNew(VirtualMachine* vm)
{
    ObjectDictionary* obj = ALLOCATE_OBJECT(vm, Dictionary);
    HashTableInit(&obj->entries);
    return obj;
}

/*
ObjectDictionary* ObjectDictionaryFromJSON(VirtualMachine* vm, ObjectString* module_path, ObjectModule* module, const cJSON* data)
{
    assert(cJSON_IsObject(data));

    ObjectDictionary* obj = ObjectDictionaryNew(vm);

    const cJSON* entry = NULL;
    cJSON_ArrayForEach(entry, data)
    {
        assert(cJSON_IsObject(entry));

        const cJSON* key_json = cJSON_GetObjectItemCaseSensitive(entry, "key");
        const cJSON* value_json = cJSON_GetObjectItemCaseSensitive(entry, "value");

        Value key = ValueFromJSON(vm, module, key_json);
        Value value = ValueFromJSON(vm, module, value_json);

        HashTablePut(&obj->entries, vm, key, value);
    }

    return obj;
}
*/

void ObjectDictionaryFree(ObjectDictionary* self, VirtualMachine* vm)
{
    HashTableDeinit(&self->entries, vm);
    FREE_OBJECT(vm, self, Dictionary);
}

void ObjectDictionaryPrint(const ObjectDictionary* self, FILE* out)
{
    // TODO: Dictionary print and custom objects.
    HashTablePrint(&self->entries, out);
}
