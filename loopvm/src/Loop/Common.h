#ifndef LOOP_CONFIGURATION_H
#define LOOP_CONFIGURATION_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "Error.h"

#include "cJSON/cJSON.h"
#include "cwalk/cwalk.h"

#define VM_STACK_SIZE_PER_FRAME 256
#define VM_FRAMES_COUNT 64
#define VM_TRACE_EXECUTION

#define DISASM_FUNC_AFTER_READING

#define HASH_TABLE_MAX_LOAD_FACTOR 0.75

#define DEBUG_MODE

#define STRESS_GC
#define LOG_GC
#define GC_HEAP_GROW_FACTOR 2

// Forgive me.
#define LOOP_PATH_MAX 4096

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
FORWARD_DECL(ObjectClass);
FORWARD_DECL(ObjectInstance);
FORWARD_DECL(ObjectBoundMethod);

#endif // LOOP_CONFIGURATION_H
