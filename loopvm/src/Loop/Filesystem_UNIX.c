#include "Filesystem.h"

#ifdef LOOP_COMPILE_UNIX

#include <limits.h>
#include <unistd.h>

#include "Objects/String.h"

ObjectString* GetCurrentWorkingDirectory(VirtualMachine* vm)
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) == NULL)
    {
        return NULL;
    }

    return ObjectStringFromLiteral(vm, buffer);
}

ObjectString* GetAbsolutePath(VirtualMachine* vm, const ObjectString* path)
{
    char buffer[PATH_MAX];
    if (realpath(path->str, buffer) == NULL)
    {
        return NULL;
    }

    return ObjectStringFromLiteral(vm, buffer);
}

ObjectString* JoinPath(VirtualMachine* vm, const ObjectString* path1, const ObjectString* path2)
{
    ObjectString* separator = ObjectStringFromLiteral(vm, "/"); // TODO: Pretty bad algorithm.
    return ObjectStringConcatenate(vm, ObjectStringConcatenate(vm, path1, separator), path2);
}

bool DoesPathExists(const ObjectString* path)
{
    return access(path->str, F_OK) == 0;
}

#endif