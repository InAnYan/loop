#ifndef LOOP_OBJECT_H
#define LOOP_OBJECT_H

#include "Common.h"

#include "Chunk.h"

#define ObjectType_LIST(o) \
    o(String) \
    o(Function)

typedef enum ObjectType
{
    #define ObjectType_ENUM(name) ObjectType_##name,

    ObjectType_LIST(ObjectType_ENUM)

    #undef ObjectType_ENUM
} ObjectType;

const char* ObjectTypeToString(ObjectType value);

typedef struct Object
{
    bool marked;
    ObjectType type;
    Object* next;
} Object;

Object* ObjectFromJSON(VirtualMachine* vm, const cJSON* json);

ObjectType ObjectGetType(const Object* self);

bool ObjectIsString(const Object* self);
bool ObjectIsFunction(const Object* self);

ObjectString* ObjectAsString(Object* self);
const ObjectString* ObjectAsStringConst(const Object* self);
ObjectFunction* ObjectAsFunction(Object* self);
const ObjectFunction* ObjectAsFunctionConst(const Object* self);

void ObjectPrint(const Object* self, FILE* out, PrintFlags flags);
size_t ObjectHash(const Object* self);

void ObjectFree(Object* self, VirtualMachine* vm);

typedef struct ObjectString
{
    Object obj;
    const char* str;
    size_t length;
    size_t hash;
} ObjectString;

ObjectString* ObjectStringNew(VirtualMachine* vm, const char* str, size_t length, size_t hash);
ObjectString* ObjectStringFromJSON(VirtualMachine* vm, const cJSON* data);
void ObjectStringFree(ObjectString* self, VirtualMachine* vm);

size_t CalculateStringHash(const char* str, size_t length);

void ObjectStringPrint(const ObjectString* self, FILE* out, PrintFlags flags);

typedef struct ObjectFunction
{
    Object obj;
    ObjectString* name;
    size_t arity;
    Chunk chunk;
} ObjectFunction;

ObjectFunction* ObjectFunctionNew(VirtualMachine* vm, ObjectString* name, size_t arity);
ObjectFunction* ObjectFunctionFromJSON(VirtualMachine* vm, const cJSON* data);
void ObjectFunctionFree(ObjectFunction* self, VirtualMachine* vm);

void ObjectFunctionPrint(const ObjectFunction* self, FILE* out, PrintFlags flags);

#endif // LOOP_OBJECT_H
