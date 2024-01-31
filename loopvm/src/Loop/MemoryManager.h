#ifndef LOOP_MEMORYMANAGER_H
#define LOOP_MEMORYMANAGER_H

#include "Common.h"

#define GROW_CAPACITY(old_capacity) ((old_capacity) < 8 ? 8 : (old_capacity) * 2)

#define REALLOC_ARRAY(vm, ptr, type, new_capacity, old_capacity) \
    (type*)MemoryManagerReallocate(&(vm)->memory_manager, (ptr), sizeof(type) * (new_capacity), sizeof(type) * (old_capacity))

#define FREE_ARRAY(vm, ptr, type, capacity) \
    MemoryManagerFree(&(vm)->memory_manager, (ptr), sizeof(type) * (capacity))

#define ALLOC_ARRAY(vm, type, capacity) \
    (type*)MemoryManagerAllocate(&(vm)->memory_manager, sizeof(type) * (capacity))

typedef struct MemoryManager
{
    Object* objects;
    VirtualMachine* vm;
    Object** gray_stack;
    size_t gray_stack_capacity;
    size_t gray_stack_count;
    size_t bytes_allocated;
    size_t next_gc;
    bool on; // Used so that when loading objects from JSON memory manager offs.
} MemoryManager;

void MemoryManagerInit(MemoryManager* self, VirtualMachine* vm);
void MemoryManagerDeinit(MemoryManager* self);
void* MemoryManagerAllocate(MemoryManager* self, size_t new_size);
void* MemoryManagerReallocate(MemoryManager* self, void* ptr, size_t new_size, size_t old_size);
void MemoryManagerFree(MemoryManager* self, const void* ptr, size_t old_size);

#endif // LOOP_MEMORYMANAGER_H
