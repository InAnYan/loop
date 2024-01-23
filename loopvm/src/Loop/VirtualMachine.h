#ifndef LOOP_VIRTUALMACHINE_H
#define LOOP_VIRTUALMACHINE_H

#include "Common.h"

#include "MemoryManager.h"
#include "Value.h"
#include "HashTable.h"

typedef struct CallFrame
{
    ObjectFunction* function;
    const uint8_t* ip;
    Value* locals;
} CallFrame;

typedef struct CommonStrings
{
    ObjectString* script;
} CommonStrings;

void CommonStringsInit(CommonStrings* self, VirtualMachine* vm);

typedef struct VirtualMachine
{
    MemoryManager memory_manager;
    CommonStrings common_strings;
    Value stack[VM_STACK_SIZE_PER_FRAME * VM_FRAMES_COUNT];
    Value* stack_ptr;
    CallFrame frames[VM_FRAMES_COUNT];
    CallFrame* frame_ptr;
    HashTable strings;
    HashTable modules;
} VirtualMachine;

void VirtualMachineInit(VirtualMachine* self);
void VirtualMachineDeinit(VirtualMachine* self);
/// Do not forget to run the script.
Error VirtualMachineLoadModule(VirtualMachine* self, ObjectString* path, ObjectModule** ptr);
Error VirtualMachineRunScript(VirtualMachine* self, ObjectFunction* script);

#endif // LOOP_VIRTUALMACHINE_H
