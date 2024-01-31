#include "Filesystem.h"

#include "Objects/String.h"

ObjectString* JoinPathImpl(VirtualMachine* vm, ...)
{
    va_list args;
    va_start(args, vm);

    char res[LOOP_PATH_MAX] = {0};

    ObjectString* path = va_arg(args, ObjectString*);
    strcpy(res, path->str);

    while ((path = va_arg(args, ObjectString*)) != NULL)
    {
        cwk_path_join(res, path->str, res, sizeof(res) / sizeof(char));
    }

    va_end(args);

    return ObjectStringFromLiteral(vm, res);
}

ObjectString* GetDirName(VirtualMachine* vm, const ObjectString* path)
{
    size_t length;
    cwk_path_get_dirname(path->str, &length);
    return ObjectStringSubstring(vm, path, 0, length);
}

ObjectString* GetBaseName(VirtualMachine* vm, const ObjectString* path)
{
    size_t length;
    const char* basename;
    cwk_path_get_basename(path->str, &basename, &length);
    return ObjectStringFromLiteral(vm, basename);
}

bool DoesPathExists(const ObjectString* path)
{
    // TODO: Is this a correct check?

    FILE* file = fopen(path->str, "r");
    if (file == NULL)
    {
        return false;
    }

    fclose(file);
    return true;
}

Error ReadFileWithComments(FILE* err_out, const char* path, char** ptr)
{
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        fprintf(err_out, "error: cannot open file '%s'\n", path);
        return Error_FileNotFound;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = malloc(file_size + 1);
    if (buffer == NULL)
    {
        fprintf(err_out, "FATAL ERROR: out of memory\n");
        return Error_OutOfMemory;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    // TODO: What to do with this check?
    /*
    if (bytes_read < file_size)
    {
        fprintf(err_out, "error: cannot read file '%s'\n", path);
        free(buffer);
        return Error_IOError;
    }
    */

    buffer[bytes_read] = '\0';
    fclose(file);
    *ptr = buffer;

    return Error_None;
}

ObjectString* RemoveExtension(VirtualMachine* vm, const ObjectString* path)
{
    size_t length;
    const char* extension;
    cwk_path_get_extension(path->str, &extension, &length);
    return ObjectStringSubstring(vm, path, 0, path->length - length);
}

#ifdef LOOP_COMPILE_UNIX

#include <linux/limits.h>

ObjectString* GetAbsolutePath(VirtualMachine* vm, const ObjectString* path)
{
    char buffer[PATH_MAX];
    if (realpath(path->str, buffer) == NULL)
    {
        return NULL;
    }

    return ObjectStringFromLiteral(vm, buffer);
}

#elif defined(LOOP_COMPILE_WINDOWS)

#include <Windows.h>

ObjectString* GetAbsolutePath(VirtualMachine* vm, const ObjectString* path)
{
    char buffer[MAX_PATH];
    if (GetFullPathName(path->str, MAX_PATH, buffer, NULL) == 0)
    {
        return NULL;
    }

    return ObjectStringFromLiteral(vm, buffer);
}

#else

#error "Don't know how to get absolute path on this platform"

#endif
