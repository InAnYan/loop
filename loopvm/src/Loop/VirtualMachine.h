#ifndef LOOP_VIRTUALMACHINE_H
#define LOOP_VIRTUALMACHINE_H

#include "Common.h"

#include "MemoryManager.h"
#include "Value.h"
#include "HashTable.h"

typedef struct VirtualMachineConfiguration
{
    FILE* user_out;
    FILE* user_err;
    FILE* debug_out;
    FILE* user_in;
} VirtualMachineConfiguration;

typedef struct CallFrame
{
    ObjectFunction* function;
    const uint8_t* ip;
    Value* locals;
} CallFrame;

typedef struct VirtualMachine
{
    VirtualMachineConfiguration conf;
    MemoryManager memory_manager;
    Value stack[VM_STACK_SIZE_PER_FRAME * VM_FRAMES_COUNT];
    Value* stack_ptr;
    CallFrame frames[VM_FRAMES_COUNT];
    CallFrame* frame_ptr;
    HashTable strings;
    HashTable globals;
} VirtualMachine;

void VirtualMachineInit(VirtualMachine* self, VirtualMachineConfiguration conf);
void VirtualMachineDeinit(VirtualMachine* self);
Value VirtualMachineRunScript(VirtualMachine* self, ObjectFunction* script);

#endif // LOOP_VIRTUALMACHINE_H
