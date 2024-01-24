#ifndef LOOP_OBJECTS_DICTIONARY_H
#define LOOP_OBJECTS_DICTIONARY_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef struct ObjectDictionary
{
    Object obj;
    HashTable entries;
} ObjectDictionary;

ObjectDictionary* ObjectDictionaryNew(VirtualMachine* vm);
void ObjectDictionaryFree(ObjectDictionary* self, VirtualMachine* vm);

void ObjectDictionaryPrint(const ObjectDictionary* self, FILE* out);

#endif // LOOP_OBJECTS_DICTIONARY_H
