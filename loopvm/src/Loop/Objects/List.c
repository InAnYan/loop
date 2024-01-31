#include "List.h"

#include "../MemoryManager.h"
#include "../VirtualMachine.h"
#include "../Value.h"

ObjectList* ObjectListNew(VirtualMachine* vm)
{
    ObjectList* obj = ALLOCATE_OBJECT(vm, List);
    obj->count = 0;
    obj->capacity = 0;
    obj->elements = NULL;
    return obj;
}

void ObjectListFree(ObjectList* self, VirtualMachine* vm)
{
    FREE_ARRAY(vm, self->elements, Value, self->capacity);
    self->count = 0;
    self->capacity = 0;
    self->elements = NULL;
    FREE_OBJECT(vm, self, List);
}

void ObjectListPush(ObjectList* self, VirtualMachine* vm, Value value)
{
    if (self->count + 1 > self->capacity)
    {
        size_t new_capacity = GROW_CAPACITY(self->capacity);
        self->elements = REALLOC_ARRAY(vm, self->elements, Value, new_capacity, self->capacity);
        self->capacity = new_capacity;
    }

    self->elements[self->count++] = value;
}

void ObjectListPrint(const ObjectList* self, FILE* out)
{
    fprintf(out, "[");
    for (size_t i = 0; i < self->count; i++)
    {
        ValuePrint(self->elements[i], out);

        if (i != self->count - 1)
        {
            fprintf(out, ", ");
        }
    }
    fprintf(out, "]");
}

void ObjectListMarkTraverse(ObjectList* self, MemoryManager* memory)
{
    for (size_t i = 0; i < self->count; i++)
    {
        ValueMark(self->elements[i], memory);
    }
}
