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

typedef struct CommonObjects
{
    ObjectString* script;
    ObjectString* init;
    ObjectString* empty_string;
    ObjectString* dot_code;
    ObjectString* compiled_dir;
} CommonObjects;

void CommonObjectsMarkTraverse(CommonObjects* self, MemoryManager* memory);

typedef struct VirtualMachine
{
    MemoryManager memory_manager;
    CommonObjects common;
    Value stack[VM_STACK_SIZE_PER_FRAME * VM_FRAMES_COUNT];
    Value* stack_ptr;
    CallFrame frames[VM_FRAMES_COUNT];
    CallFrame* frame_ptr;
    HashTable strings;
    HashTable modules;
    ObjectString* called_path;
    ObjectString* packages_path;
} VirtualMachine;

Error VirtualMachineInit(VirtualMachine* self);
void VirtualMachineDeinit(VirtualMachine* self);
/// Do not forget to run the script.
Error VirtualMachineLoadModule(VirtualMachine* self, ObjectString* parent, ObjectString* path, ObjectModule** ptr);
Error VirtualMachineRunScript(VirtualMachine* self, ObjectFunction* script);

void VirtualMachineMarkRoots(VirtualMachine* self, MemoryManager* memory);

#endif // LOOP_VIRTUALMACHINE_H
