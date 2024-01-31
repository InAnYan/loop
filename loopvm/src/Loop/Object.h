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

#define ALLOCATE_OBJECT_CHILD(vm, parent, child) \
    (Object##child*)ObjectAllocateRaw(vm, ObjectType_##parent, sizeof(Object##child))

#define FREE_OBJECT(vm, self, name) \
    ObjectFreeRaw(vm, (Object*)self, sizeof(Object##name))

/// Use this function with caution. module_path will be set to NULL.
Object* ObjectFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* json);

ObjectType ObjectGetType(const Object* self);

bool ObjectIsString(const Object* self);
bool ObjectIsFunction(const Object* self);
bool ObjectIsModule(const Object* self);
bool ObjectIsDictionary(const Object* self);
bool ObjectIsClass(const Object* self);
bool ObjectIsInstance(const Object* self);
bool ObjectIsBoundMethod(const Object* self);
bool ObjectIsUpvalue(const Object* self);
bool ObjectIsClosure(const Object* self);

ObjectString* ObjectAsString(Object* self);
ObjectFunction* ObjectAsFunction(Object* self);
ObjectModule* ObjectAsModule(Object* self);
ObjectDictionary* ObjectAsDictionary(Object* self);
ObjectClass* ObjectAsClass(Object* self);
ObjectInstance* ObjectAsInstance(Object* self);
ObjectBoundMethod* ObjectAsBoundMethod(Object* self);
ObjectUpvalue* ObjectAsUpvalue(Object* self);
ObjectClosure* ObjectAsClosure(Object* self);

const ObjectString* ObjectAsStringConst(const Object* self);
const ObjectFunction* ObjectAsFunctionConst(const Object* self);
const ObjectModule* ObjectAsModuleConst(const Object* self);
const ObjectDictionary* ObjectAsDictionaryConst(const Object* self);
const ObjectClass* ObjectAsClassConst(const Object* self);
const ObjectInstance* ObjectAsInstanceConst(const Object* self);
const ObjectBoundMethod* ObjectAsBoundMethodConst(const Object* self);
const ObjectUpvalue* ObjectAsUpvalueConst(const Object* self);
const ObjectClosure* ObjectAsClosureConst(const Object* self);

void ObjectPrint(const Object* self, FILE* out);

// TODO: Should I pass there VM or MemoryManager?
void ObjectFree(Object* self, VirtualMachine* vm);

void ObjectMark(Object* self, MemoryManager* memory);
void ObjectMarkMaybeNull(Object* self, MemoryManager* memory);
void ObjectMarkTraverse(Object* self, MemoryManager* memory);

#endif // LOOP_OBJECT_H
