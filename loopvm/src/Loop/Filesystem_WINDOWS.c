#include "Filesystem.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"
#include "Objects/String.h"

#ifdef LOOP_COMPILE_WINDOWS

#include <windows.h>

ObjectString* GetCurrentWorkingDirectory(VirtualMachine* vm)
{
    // TODO: Hope it's right.
    DWORD buf_len = GetCurrentDirectory(0, NULL);
    char* buffer = ALLOC_ARRAY(vm, char, buf_len);
    if (GetCurrentDirectory(buf_len, buffer) == 0)
    {
        FREE_ARRAY(vm, buffer, char, buf_len);
        return NULL;
    }
    return ObjectStringNew(vm, buffer, buf_len - 1, CalculateStringHash(buffer, buf_len - 1));
}

ObjectString* GetAbsolutePath(VirtualMachine* vm, const ObjectString* path)
{
    char buffer[MAX_PATH];
    if (GetFullPathName(path->str, MAX_PATH, buffer, NULL) == 0)
    {
        return NULL;
    }

    return ObjectStringFromLiteral(vm, buffer);
}

ObjectString* JoinPath(VirtualMachine* vm, const ObjectString* path1, const ObjectString* path2)
{
    ObjectString* separator = ObjectStringFromLiteral(vm, "\\"); // TODO: Pretty bad algorithm.
    return ObjectStringConcatenate(vm, ObjectStringConcatenate(vm, path1, separator), path2);
}

bool DoesPathExists(const ObjectString* path)
{
    return GetFileAttributes(path->str) != INVALID_FILE_ATTRIBUTES;
}

#endif
