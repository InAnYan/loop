#ifndef LOOP_OBJECT_H
#define LOOP_OBJECT_H

#include "Common.h"

#include "Chunk.h"
#include "HashTable.h"

#define ObjectType_LIST(o) \
    o(String) \
    o(Function) \
    o(Module) \
    o(Dictionary)

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

Object* ObjectFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* json);

ObjectType ObjectGetType(const Object* self);

bool ObjectIsString(const Object* self);
bool ObjectIsFunction(const Object* self);
bool ObjectIsModule(const Object* self);
bool ObjectIsDictionary(const Object* self);

ObjectString* ObjectAsString(Object* self);
ObjectFunction* ObjectAsFunction(Object* self);
ObjectModule* ObjectAsModule(Object* self);
ObjectDictionary* ObjectAsDictionary(Object* self);

const ObjectString* ObjectAsStringConst(const Object* self);
const ObjectFunction* ObjectAsFunctionConst(const Object* self);
const ObjectModule* ObjectAsModuleConst(const Object* self);
const ObjectDictionary* ObjectAsDictionaryConst(const Object* self);

void ObjectPrint(const Object* self, FILE* out, PrintFlags flags);

void ObjectFree(Object* self, VirtualMachine* vm);

typedef struct ObjectModule
{
    Object obj;
    ObjectString* name;
    ObjectString* path;
    ObjectFunction* script;
    HashTable exports;
    HashTable globals;
    bool is_partial;
} ObjectModule;

ObjectModule* ObjectModuleNew(VirtualMachine* vm, ObjectString* name, ObjectString* path);
/// module can be NULL.
ObjectModule* ObjectModuleFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectModuleFree(ObjectModule* self, VirtualMachine* vm);

void ObjectModulePrint(const ObjectModule* self, FILE* out, PrintFlags flags);

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

void ObjectStringPrint(const ObjectString* self, FILE* out, PrintFlags flags);

typedef struct ObjectFunction
{
    Object obj;
    ObjectModule* module;
    ObjectString* name;
    size_t arity;
    Chunk chunk;
} ObjectFunction;

ObjectFunction* ObjectFunctionNew(VirtualMachine* vm, ObjectModule* module, ObjectString* name, size_t arity);
ObjectFunction* ObjectFunctionFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectFunctionFree(ObjectFunction* self, VirtualMachine* vm);

void ObjectFunctionPrint(const ObjectFunction* self, FILE* out, PrintFlags flags);

typedef struct ObjectDictionary
{
    Object obj;
    HashTable entries;
} ObjectDictionary;

ObjectDictionary* ObjectDictionaryNew(VirtualMachine* vm);
ObjectDictionary* ObjectDictionaryFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* data);
void ObjectDictionaryFree(ObjectDictionary* self, VirtualMachine* vm);

void ObjectDictionaryPrint(const ObjectDictionary* self, FILE* out, PrintFlags flags);

#endif // LOOP_OBJECT_H
