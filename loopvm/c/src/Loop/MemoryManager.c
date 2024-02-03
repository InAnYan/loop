#include "MemoryManager.h"

#include "Object.h"
#include "VirtualMachine.h"

static void FreeAllObjects(MemoryManager *self);

void MemoryManagerInit(MemoryManager *self, VirtualMachine *vm) {
    self->objects = NULL;
    self->vm = vm;
    self->gray_stack = NULL;
    self->gray_stack_capacity = 0;
    self->gray_stack_count = 0;
    self->bytes_allocated = 0;
    self->next_gc = 1024 * 1024;
    self->on = false;
}

void MemoryManagerDeinit(MemoryManager *self) {
    FreeAllObjects(self);
    free(self->gray_stack);
    MemoryManagerInit(self, NULL);
}

static void FreeAllObjects(MemoryManager *self) {
    Object *current = self->objects;
    while (current != NULL) {
        Object *next = current->next;
        ObjectFree(current, self->vm);
        current = next;
    }
}

void *MemoryManagerAllocate(MemoryManager *self, size_t new_size) {
    return MemoryManagerReallocate(self, NULL, new_size, 0);
}

static void CollectGarbage(MemoryManager *self);

void *MemoryManagerReallocate(MemoryManager *self, void *ptr, size_t new_size, size_t old_size) {
#ifdef GC_STRESS
    if (self->on && new_size > old_size) {
        CollectGarbage(self);
    }
#else
    if (self->on && self->bytes_allocated > self->next_gc)
    {
        CollectGarbage(self);
    }
#endif

    self->bytes_allocated += new_size - old_size;

    if (new_size == 0) {
        free(ptr);
        return NULL;
    }

    void *res = realloc(ptr, new_size);
    if (res == NULL) {
        fprintf(stderr, "FATAL ERROR: Failed to reallocate memory\n");
        exit(1);
    }

    return res;
}

void MemoryManagerFree(MemoryManager *self, const void *ptr, size_t old_size) {
    MemoryManagerReallocate(self, (void *) ptr, 0, old_size);
}

static void MarkStage(MemoryManager *self);

static void SweepStage(MemoryManager *self);

static void UpdateNextGC(MemoryManager *self);

static void CollectGarbage(MemoryManager *self) {
#ifdef GC_LOG
    fprintf(DEBUG_OUT, "== GC: Begin.\n");
    size_t before = self->bytes_allocated;
#endif

    MarkStage(self);
    // Pretty bad code. It is part of VM, but it is there.
    HashTableRemoveWhite(&self->vm->strings, self);
    HashTableRemoveWhite(&self->vm->modules, self);
    SweepStage(self);
    UpdateNextGC(self);

#ifdef GC_LOG
    fprintf(DEBUG_OUT, "== GC: End.\n");
    fprintf(DEBUG_OUT, "==     Collected %zu bytes (from %zu to %zu), next at %zu.\n",
            before - self->bytes_allocated, before, self->bytes_allocated, self->next_gc);
#endif
}

static void TraverseRoots(MemoryManager *self);

static void MarkStage(MemoryManager *self) {
    VirtualMachineMarkRoots(self->vm, self);
    TraverseRoots(self);
}

static void TraverseRoots(MemoryManager *self) {
    while (self->gray_stack_count) {
        Object *obj = self->gray_stack[--self->gray_stack_count];
        ObjectMarkTraverse(obj, self);
    }
}

static void SweepStage(MemoryManager *self) {
    Object *previous = NULL;
    Object *object = self->objects;

    while (object != NULL) {
        if (object->marked) {
            object->marked = false;
            previous = object;
            object = object->next;
        } else {
            Object *unreached = object;
            object = object->next;

            if (previous != NULL) {
                previous->next = object;
            } else {
                self->objects = object;
            }

            ObjectFree(unreached, self->vm);
        }
    }
}

static void UpdateNextGC(MemoryManager *self) {
    self->next_gc *= GC_HEAP_GROW_FACTOR;
}
