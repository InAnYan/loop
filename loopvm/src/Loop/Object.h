#ifndef LOOP_OBJECT_H
#define LOOP_OBJECT_H

#include "Common.h"

#include "ObjectType.h"

typedef struct Object {
    bool marked;
    ObjectType type;
    Object *next;
} Object;

Object *ObjectAllocateRaw(VirtualMachine *vm, ObjectType type, size_t size);

void ObjectFreeRaw(VirtualMachine *vm, Object *obj, size_t size);

#define ALLOCATE_OBJECT(vm, name) \
    (Object##name*)ObjectAllocateRaw(vm, ObjectType_##name, sizeof(Object##name))

#define ALLOCATE_OBJECT_CHILD(vm, parent, child) \
    (Object##child*)ObjectAllocateRaw(vm, ObjectType_##parent, sizeof(Object##child))

#define FREE_OBJECT(vm, self, name) \
    ObjectFreeRaw(vm, (Object*)self, sizeof(Object##name))

/// Use this function with caution. module_path will be set to NULL.
Object *ObjectFromJSON(VirtualMachine *vm, ObjectModule *module, const cJSON *json);

ObjectType ObjectGetType(const Object *self);

#define OBJECT_IS_DECL(name) \
    bool ObjectIs##name(const Object* self);

ObjectType_LIST(OBJECT_IS_DECL)

#undef OBJECT_IS_DECL

#define OBJECT_AS_DECL(name) \
    Object##name* ObjectAs##name(Object* self);

ObjectType_LIST(OBJECT_AS_DECL)

#undef OBJECT_AS_DECL

#define OBJECT_AS_CONST_DECL(name) \
    const Object##name* ObjectAs##name##Const(const Object* self);

ObjectType_LIST(OBJECT_AS_CONST_DECL)

#undef OBJECT_AS_CONST_DECL

void ObjectPrint(const Object *self, FILE *out);

// TODO: Should I pass there VM or MemoryManager?
void ObjectFree(Object *self, VirtualMachine *vm);

void ObjectMark(Object *self, MemoryManager *memory);

void ObjectMarkMaybeNull(Object *self, MemoryManager *memory);

void ObjectMarkTraverse(Object *self, MemoryManager *memory);

#endif // LOOP_OBJECT_H
