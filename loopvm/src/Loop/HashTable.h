#ifndef LOOP_HASHTABLE_H
#define LOOP_HASHTABLE_H

#include "Common.h"

#include "Value.h"

typedef struct HashTableEntry
{
    Value key;
    Value value;
} HashTableEntry;

typedef struct HashTable
{
    HashTableEntry* entries;
    size_t count;
    size_t capacity;
} HashTable;

void HashTableInit(HashTable* self);
void HashTableInitWithCapacity(HashTable* self, VirtualMachine* vm);
void HashTableDeinit(HashTable* self, VirtualMachine* vm);
bool HashTablePut(HashTable* self, VirtualMachine* vm, Value key, Value value);
void HashTableAddAll(HashTable* self, VirtualMachine* vm, HashTable* other);
bool HashTableGet(HashTable* self, Value key, Value* value);
bool HashTableGetStringKey(HashTable* self, const char* key, size_t length, size_t hash, ObjectString** ptr);
bool HashTableDelete(HashTable* self, Value key);
void HashTablePrint(const HashTable* self, FILE* out);

#endif // LOOP_HASHTABLE_H
