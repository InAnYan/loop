#include "HashTable.h"

#include "MemoryManager.h"
#include "Object.h"
#include "VirtualMachine.h"

void HashTableInit(HashTable* self)
{
    self->capacity = 0;
    self->count = 0;
    self->entries = NULL;
}

void HashTableDeinit(HashTable* self, VirtualMachine* vm)
{
    FREE_ARRAY(vm, self->entries, HashTableEntry, self->capacity);
    HashTableInit(self);
}

static void AdjustCapacity(HashTable* self, VirtualMachine* vm, size_t new_capacity);
static HashTableEntry* FindEntry(HashTableEntry* entries, size_t capacity, Value key);

bool HashTablePut(HashTable* self, VirtualMachine* vm, Value key, Value value)
{
    if (self->count + 1 > HASH_TABLE_MAX_LOAD_FACTOR * self->capacity)
    {
        size_t new_capacity = GROW_CAPACITY(self->capacity);
        AdjustCapacity(self, vm, new_capacity);
    }

    HashTableEntry* entry = FindEntry(self->entries, self->capacity, key);

    if (ValueIsNull(entry->key))
    {
        entry->key = key;
        entry->value = value;

        if (ValueIsNull(entry->value))
        {
            self->count++;
        }

        return true;
    }

    entry->value = value;
    return false;
}

static void AdjustCapacity(HashTable* self, VirtualMachine* vm, size_t new_capacity)
{
    HashTableEntry* entries = ALLOC_ARRAY(vm, HashTableEntry, new_capacity);

    for (size_t i = 0; i < self->capacity; ++i)
    {
        HashTableEntry* entry = &entries[i];
        entry->key = ValueNull();
        entry->value = ValueNull();
    }

    size_t new_count = 0;
    for (size_t i = 0; i < self->capacity; ++i)
    {
        HashTableEntry* entry = &self->entries[i];
        if (!ValueIsNull(entry->key))
        {
            HashTableEntry* dest = FindEntry(self->entries, self->capacity, entry->key);
            dest->key = entry->key;
            dest->value = entry->value;
            ++new_count;
        }
    }

    FREE_ARRAY(vm, self->entries, HashTableEntry, self->capacity);
    self->entries = entries;
    self->capacity = new_capacity;
    self->count = new_count;
}

static HashTableEntry* FindEntry(HashTableEntry* entries, size_t capacity, Value key)
{
    size_t hash = ValueHash(key);
    size_t index = hash % capacity;
    HashTableEntry* thombstone = NULL;

    while (true)
    {
        HashTableEntry* entry = &entries[index];

        if (ValueAreEqual(entry->key, key))
        {
            return entry;
        }

        if (ValueIsNull(entry->key))
        {
            if (ValueIsNull(entry->value))
            {
                return thombstone == NULL ? entry : thombstone;
            }

            thombstone = entry;
        }

        index = (index + 1) % capacity;
    }
}

void HashTableAddAll(HashTable* self, VirtualMachine* vm, HashTable* other)
{
    for (size_t i = 0; i < other->capacity; ++i)
    {
        HashTableEntry* entry = &other->entries[i];
        if (!ValueIsNull(entry->key))
        {
            HashTablePut(self, vm, entry->key, entry->value);
        }
    }
}

bool HashTableGet(HashTable* self, Value key, Value* value)
{
    HashTableEntry* entry = FindEntry(self->entries, self->capacity, key);

    if (ValueIsNull(entry->key))
    {
        return false;
    }

    *value = entry->value;
    return true;
}

bool HashTableGetStringKey(HashTable* self, const char* key_str, size_t length, size_t hash, ObjectString** ptr)
{
    for (size_t i = 0; i < self->capacity; ++i)
    {
        HashTableEntry* entry = &self->entries[i];
        Value key = entry->key;

        if (ValueIsObject(key))
        {
            Object* obj = ValueAsObject(key);

            if (ObjectIsString(obj))
            {
                ObjectString* str = ObjectAsString(obj);

                if (str->hash == hash && str->length == length && memcmp(str->str, key_str, length) == 0)
                {
                    *ptr = str;
                    return true;
                }
            }
        }
    }

    return false;
}

bool HashTableDelete(HashTable* self, Value key)
{
    HashTableEntry* entry = FindEntry(self->entries, self->capacity, key);

    if (ValueIsNull(entry->key))
    {
        return false;
    }

    entry->key = ValueNull();
    entry->value = ValueBool(true);
    return true;
}
