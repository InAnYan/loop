cmake_minimum_required(VERSION 3.22)
project(loopvm C)

set(CMAKE_C_STANDARD 11)

add_executable(loopvm
        src/main.c
        src/Loop/Chunk.c
        src/Loop/Chunk.h
        src/Loop/Object.c
        src/Loop/Object.h
        src/Loop/Opcode.c
        src/Loop/Opcode.h
        src/Loop/Value.c
        src/Loop/Value.h
        src/Loop/VirtualMachine.c
        src/Loop/VirtualMachine.h
        src/Loop/HashTable.h
        src/Loop/HashTable.c
        src/Loop/Common.h
        src/Loop/MemoryManager.h
        src/Loop/MemoryManager.c
        src/Loop/PrintFlags.h
        src/libs/cJSON/cJSON.h
        src/libs/cJSON/cJSON.c
        src/libs/cJSON/cJSON.c
        src/Loop/Error.h
        src/Loop/Error.c
)
target_include_directories(loopvm PRIVATE src/)