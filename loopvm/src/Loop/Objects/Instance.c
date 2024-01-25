#include "Instance.h"

#include "Class.h"
#include "Module.h"
#include "String.h"

ObjectInstance* ObjectInstanceNew(VirtualMachine* vm, ObjectClass* klass)
{
    ObjectInstance* obj = ALLOCATE_OBJECT(vm, Instance);

    obj->klass = klass;
    HashTableInit(&obj->fields);

    return obj;
}

void ObjectInstanceFree(ObjectInstance* self, VirtualMachine* vm)
{
    self->klass = NULL;
    HashTableDeinit(&self->fields, vm);
    FREE_OBJECT(vm, self, Instance);
}

void ObjectInstancePrint(const ObjectInstance* self, FILE* out)
{
    fprintf(out, "<instance of %s.%s>",
            self->klass->module->name->str, self->klass->name->str);
}

void ObjectInstanceMarkTraverse(ObjectInstance* self, MemoryManager* memory)
{
    ObjectMark((Object*)self->klass, memory);
    HashTableMark(&self->fields, memory);
}
