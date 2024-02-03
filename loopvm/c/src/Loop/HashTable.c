#include "HashTable.h"

#include "MemoryManager.h"
#include "Object.h"
#include "VirtualMachine.h"

#include "Objects/String.h"

void HashTableInit(HashTable *self) {
    self->capacity = 0;
    self->count = 0;
    self->entries = NULL;
}

static void AdjustCapacity(HashTable *self, VirtualMachine *vm, size_t new_capacity);

void HashTableInitWithCapacity(HashTable *self, VirtualMachine *vm) {
    HashTableInit(self);
    AdjustCapacity(self, vm, GROW_CAPACITY(self->capacity));
}

void HashTableDeinit(HashTable *self, VirtualMachine *vm) {
    FREE_ARRAY(vm, self->entries, HashTableEntry, self->capacity);
    HashTableInit(self);
}

static HashTableEntry *FindEntry(HashTableEntry *entries, size_t capacity, Value key);

bool HashTablePut(HashTable *self, VirtualMachine *vm, Value key, Value value) {
    if (self->count + 1 > HASH_TABLE_MAX_LOAD_FACTOR * self->capacity) {
        size_t new_capacity = GROW_CAPACITY(self->capacity);
        AdjustCapacity(self, vm, new_capacity);
    }

    HashTableEntry *entry = FindEntry(self->entries, self->capacity, key);

    if (ValueIsNull(entry->key)) {
        if (ValueIsNull(entry->value)) {
            self->count++;
        }

        entry->key = key;
        entry->value = value;
        return true;
    }

    entry->value = value;
    return false;
}

static void AdjustCapacity(HashTable *self, VirtualMachine *vm, size_t new_capacity) {
    HashTableEntry *entries = ALLOC_ARRAY(vm, HashTableEntry, new_capacity);

    for (size_t i = 0; i < new_capacity; ++i) {
        HashTableEntry *entry = &entries[i];
        entry->key = ValueNull();
        entry->value = ValueNull();
    }

    size_t new_count = 0;
    for (size_t i = 0; i < self->capacity; ++i) {
        HashTableEntry *entry = &self->entries[i];
        if (!ValueIsNull(entry->key)) {
            HashTableEntry *dest = FindEntry(entries, new_capacity, entry->key);
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

static HashTableEntry *FindEntry(HashTableEntry *entries, size_t capacity, Value key) {
    size_t hash = ValueHash(key);
    size_t index = hash % capacity;
    HashTableEntry *thombstone = NULL;

    while (true) {
        HashTableEntry *entry = &entries[index];

        if (ValueAreEqual(entry->key, key)) {
            return entry;
        }

        if (ValueIsNull(entry->key)) {
            if (ValueIsNull(entry->value)) {
                return thombstone == NULL ? entry : thombstone;
            }

            thombstone = entry;
        }

        index = (index + 1) % capacity;
    }
}

void HashTableAddAll(HashTable *self, VirtualMachine *vm, HashTable *other) {
    for (size_t i = 0; i < other->capacity; ++i) {
        HashTableEntry *entry = &other->entries[i];
        if (!ValueIsNull(entry->key)) {
            HashTablePut(self, vm, entry->key, entry->value);
        }
    }
}

bool HashTableGet(HashTable *self, Value key, Value *value) {
    if (self->count == 0) {
        return false;
    }

    HashTableEntry *entry = FindEntry(self->entries, self->capacity, key);

    if (ValueIsNull(entry->key)) {
        return false;
    }

    *value = entry->value;
    return true;
}

bool HashTableGetStringKey(HashTable *self, const char *key_str, size_t length, size_t hash, ObjectString **ptr) {
    // TODO: This is bad. Because it is not like hash table lookup. Also for strings interning no need for these checks.

    for (size_t i = 0; i < self->capacity; ++i) {
        HashTableEntry *entry = &self->entries[i];
        Value key = entry->key;

        if (ValueIsObject(key)) {
            Object *obj = ValueAsObject(key);

            if (ObjectIsString(obj)) {
                ObjectString *str = ObjectAsString(obj);

                if (str->hash == hash && str->length == length && memcmp(str->str, key_str, length) == 0) {
                    *ptr = str;
                    return true;
                }
            }
        }
    }

    return false;
}

bool HashTableDelete(HashTable *self, Value key) {
    HashTableEntry *entry = FindEntry(self->entries, self->capacity, key);

    if (ValueIsNull(entry->key)) {
        return false;
    }

    entry->key = ValueNull();
    entry->value = ValueBool(true);
    return true;
}

static void PrintEntries(const HashTable *self, FILE *out);

void HashTablePrint(const HashTable *self, FILE *out) {
    if (self->count == 0) {
        fprintf(out, "{}");
        return;
    }

    fprintf(out, "{ ");
    PrintEntries(self, out);
    fprintf(out, " }");
}

static void PrintEntries(const HashTable *self, FILE *out) {
    size_t length = 0;

    for (size_t i = 0; i < self->capacity; ++i) {
        const HashTableEntry *entry = &self->entries[i];

        if (!ValueIsNull(entry->key)) {
            ++length;

            ValuePrint(entry->key, out);
            fprintf(out, ": ");
            ValuePrint(entry->value, out);

            if (length != self->count) {
                fprintf(out, ", ");
            }
        }
    }
}

void HashTableMark(HashTable *self, MemoryManager *memory) {
    for (size_t i = 0; i < self->capacity; ++i) {
        HashTableEntry *entry = &self->entries[i];
        ValueMark(entry->key, memory);
        ValueMark(entry->value, memory);
    }
}

void HashTableRemoveWhite(HashTable *self, MemoryManager *memory) {
    for (size_t i = 0; i < self->capacity; ++i) {
        HashTableEntry *entry = &self->entries[i];

        if (ValueIsObject(entry->key) && !ValueAsObject(entry->key)->marked) {
            bool res = HashTableDelete(self, entry->key);
            assert(res);
        }
    }
}
