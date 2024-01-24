#ifndef LOOP_OBJECTS_INSTANCE_H
#define LOOP_OBJECTS_INSTANCE_H

#include "../Common.h"
#include "../Object.h"

#include "../HashTable.h"

typedef struct ObjectInstance
{
    Object obj;
    ObjectClass* klass;
    HashTable fields;
} ObjectInstance;

ObjectInstance* ObjectInstanceNew(VirtualMachine* vm, ObjectClass* klass);
void ObjectInstanceFree(ObjectInstance* self, VirtualMachine* vm);

void ObjectInstancePrint(const ObjectInstance* self, FILE* out);

#endif // LOOP_OBJECTS_INSTANCE_H
