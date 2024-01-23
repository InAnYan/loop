#ifndef LOOP_CONFIGURATION_H
#define LOOP_CONFIGURATION_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Error.h"

#include "cJSON/cJSON.h"

#define VM_STACK_SIZE_PER_FRAME 256
#define VM_FRAMES_COUNT 64
#define VM_TRACE_EXECUTION

#define DISASM_CHUNKS_AFTER_READING

#define HASH_TABLE_MAX_LOAD_FACTOR 0.75

#define DEBUG_MODE

// In future, the better idea is to have a VirtualMachineConfiguration struct.

#define USER_OUT stdout
#define USER_IN stdin
#define USER_ERR stderr
#define DEBUG_OUT stderr

#define FORWARD_DECL(name) typedef struct name name

FORWARD_DECL(VirtualMachine);
FORWARD_DECL(MemoryManager);
FORWARD_DECL(Value);
FORWARD_DECL(Object);
FORWARD_DECL(ObjectString);
FORWARD_DECL(ObjectFunction);
FORWARD_DECL(ObjectModule);
FORWARD_DECL(ObjectDictionary);

#endif // LOOP_CONFIGURATION_H
