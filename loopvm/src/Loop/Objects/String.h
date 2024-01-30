#ifndef LOOP_OBJECTS_STRING_H
#define LOOP_OBJECTS_STRING_H

#include "../Common.h"
#include "../Object.h"

typedef struct ObjectString
{
    Object obj;
    char* str;
    size_t length;
    size_t hash;
} ObjectString;

ObjectString* ObjectStringNew(VirtualMachine* vm, char* str, size_t length, size_t hash);
ObjectString* ObjectStringFromLiteral(VirtualMachine* vm, const char* str);
ObjectString* ObjectStringFromJSON(VirtualMachine* vm, const cJSON* data);
void ObjectStringFree(ObjectString* self, VirtualMachine* vm);

size_t CalculateStringHash(const char* str, size_t length);
ObjectString* ObjectStringConcatenate(VirtualMachine* vm, const ObjectString* left, const ObjectString* right);
ObjectString* ObjectStringSubstring(VirtualMachine* vm, const ObjectString* str, size_t start, size_t end);

void ObjectStringPrint(const ObjectString* self, FILE* out);

void ObjectStringMarkTraverse(ObjectString* self, MemoryManager* memory);

#endif // LOOP_OBJECTS_STRING_H
