#ifndef LOOP_OBJECT_H
#define LOOP_OBJECT_H

#include "Common.h"

#include "ObjectType.h"

typedef struct Object
{
    bool marked;
    ObjectType type;
    Object* next;
} Object;

Object* ObjectAllocateRaw(VirtualMachine* vm, ObjectType type, size_t size);
void ObjectFreeRaw(VirtualMachine* vm, Object* obj, size_t size);

#define ALLOCATE_OBJECT(vm, name) \
    (Object##name*)ObjectAllocateRaw(vm, ObjectType_##name, sizeof(Object##name))

#define FREE_OBJECT(vm, self, name) \
    ObjectFreeRaw(vm, (Object*)self, sizeof(Object##name))

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

void ObjectPrint(const Object* self, FILE* out);

void ObjectFree(Object* self, VirtualMachine* vm);

#endif // LOOP_OBJECT_H
