#include "MemoryManager.h"

#include "Object.h"
#include "VirtualMachine.h"

static void FreeAllObjects(MemoryManager* self);

void MemoryManagerInit(MemoryManager* self, VirtualMachine* vm)
{
    self->objects = NULL;
    self->vm = vm;
    self->debug_out = DEBUG_OUT;
}

void MemoryManagerDeinit(MemoryManager* self)
{
    FreeAllObjects(self);
    self->objects = NULL;
    self->vm = NULL;
    self->debug_out = NULL;
}

static void FreeAllObjects(MemoryManager* self)
{
    Object* current = self->objects;
    while (current != NULL)
    {
        Object* next = current->next;
        ObjectFree(current, self->vm);
        current = next;
    }
}

void* MemoryManagerAllocate(MemoryManager* self, size_t new_size)
{
    return MemoryManagerReallocate(self, NULL, new_size, 0);
}

void* MemoryManagerReallocate(MemoryManager* self, void* ptr, size_t new_size, size_t old_size)
{
    if (new_size == 0)
    {
        free(ptr);
        return NULL;
    }

    void* res = realloc(ptr, new_size);
    if (res == NULL)
    {
        fprintf(stderr, "FATAL ERROR: Failed to reallocate memory\n");
        exit(1);
    }

    return res;
}

void MemoryManagerFree(MemoryManager* self, const void* ptr, size_t old_size)
{
    MemoryManagerReallocate(self, (void*)ptr, 0, old_size);
}
