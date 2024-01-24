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
        src/libs/cJSON/cJSON.h
        src/libs/cJSON/cJSON.c
        src/Loop/Error.h
        src/Loop/Error.c
        src/Loop/Objects/Module.h
        src/Loop/Objects/String.h
        src/Loop/Objects/Function.h
        src/Loop/Objects/Dictionary.h
        src/Loop/ObjectType.h
        src/Loop/Objects/String.c
        src/Loop/ObjectType.c
        src/Loop/Objects/Module.c
        src/Loop/Objects/Function.c
        src/Loop/Objects/Dictionary.c
        src/Loop/Objects/Class.c
        src/Loop/Objects/Instance.h
        src/Loop/Objects/Instance.c
        src/Loop/Objects/BoundMethod.h
        src/Loop/Objects/BoundMethod.c
        src/Loop/Filesystem.h
        src/Loop/Filesystem_UNIX.c
        src/Loop/Filesystem.c
        src/Loop/Filesystem_WINDOWS.c
)
target_include_directories(loopvm PRIVATE src/libs)

if(WIN32)
    add_compile_definitions(LOOP_COMPILE_WINDOWS)
endif (WIN32)

if(UNIX)
    add_compile_definitions(LOOP_COMPILE_UNIX)
endif (UNIX)
