cmake_minimum_required(VERSION 3.22)
project(loopvm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

add_library(cJSON
        c/src/libs/cJSON/cJSON.c
        c/src/libs/cJSON/cJSON.h
)
target_include_directories(cJSON PRIVATE c/src/libs/cJSON)

add_library(cwalk
        c/src/libs/cwalk/cwalk.c
        c/src/libs/cwalk/cwalk.h
)
target_include_directories(cwalk PRIVATE c/src/libs/cwalk)

add_executable(loopvm
        c/src/main.c
        c/src/Loop/Chunk.c
        c/src/Loop/Chunk.h
        c/src/Loop/Object.c
        c/src/Loop/Object.h
        c/src/Loop/Opcode.c
        c/src/Loop/Opcode.h
        c/src/Loop/Value.c
        c/src/Loop/Value.h
        c/src/Loop/VirtualMachine.c
        c/src/Loop/VirtualMachine.h
        c/src/Loop/HashTable.h
        c/src/Loop/HashTable.c
        c/src/Loop/Common.h
        c/src/Loop/MemoryManager.h
        c/src/Loop/MemoryManager.c
        c/src/Loop/Error.h
        c/src/Loop/Error.c
        c/src/Loop/Objects/Module.h
        c/src/Loop/Objects/String.h
        c/src/Loop/Objects/Function.h
        c/src/Loop/Objects/Dictionary.h
        c/src/Loop/ObjectType.h
        c/src/Loop/Objects/String.c
        c/src/Loop/ObjectType.c
        c/src/Loop/Objects/Module.c
        c/src/Loop/Objects/Function.c
        c/src/Loop/Objects/Dictionary.c
        c/src/Loop/Objects/Class.c
        c/src/Loop/Objects/Instance.h
        c/src/Loop/Objects/Instance.c
        c/src/Loop/Objects/BoundMethod.h
        c/src/Loop/Objects/BoundMethod.c
        c/src/Loop/Filesystem.h
        c/src/Loop/Filesystem.c
        c/src/Loop/Objects/Upvalue.h
        c/src/Loop/Objects/Closure.h
        c/src/Loop/Objects/Closure.c
        c/src/Loop/Objects/Upvalue.c
        c/src/Loop/Objects/List.h
        c/src/Loop/Objects/List.c
)
target_include_directories(loopvm PRIVATE c/src/libs)
target_link_libraries(loopvm PRIVATE cJSON cwalk)

if (WIN32)
    add_compile_definitions(LOOP_COMPILE_WINDOWS)
endif (WIN32)

if (UNIX)
    add_compile_definitions(LOOP_COMPILE_UNIX)
endif (UNIX)
