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
/// module can be NULL.
ObjectString* ObjectStringFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectStringFree(ObjectString* self, VirtualMachine* vm);

size_t CalculateStringHash(const char* str, size_t length);

void ObjectStringPrint(const ObjectString* self, FILE* out);

#endif // LOOP_OBJECTS_STRING_H
