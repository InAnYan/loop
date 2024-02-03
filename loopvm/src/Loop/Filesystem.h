#ifndef LOOP_FILESYSTEM_H
#define LOOP_FILESYSTEM_H

#include "Common.h"

// God, forgive me for this inefficient interface.

#define JoinPath(vm, path1, ...) JoinPathImpl(vm, path1, __VA_ARGS__, NULL)

ObjectString *JoinPathImpl(VirtualMachine *vm, ...);

ObjectString *GetDirName(VirtualMachine *vm, const ObjectString *path);

ObjectString *GetBaseName(VirtualMachine *vm, const ObjectString *path);

/// Uses 'malloc'. Free the returned buffer with 'free' if there is no error.
Error ReadFileWithComments(FILE *err_out, const char *path, char **ptr);

// TODO: TOCTOU vulnerability?
bool DoesPathExists(const ObjectString *path);

/// May return NULL.
ObjectString *GetAbsolutePath(VirtualMachine *vm, const ObjectString *path);

ObjectString *RemoveExtension(VirtualMachine *vm, const ObjectString *path);

#endif // LOOP_FILESYSTEM_H
