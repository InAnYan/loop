#ifndef LOOP_FILESYSTEM_H
#define LOOP_FILESYSTEM_H

#include "Common.h"

/// May return NULL.
ObjectString* GetCurrentWorkingDirectory(VirtualMachine* vm);
/// May return NULL.
ObjectString* GetAbsolutePath(VirtualMachine* vm, const ObjectString* path);
ObjectString* JoinPath(VirtualMachine* vm, const ObjectString* path1, const ObjectString* path2);

/// Uses 'malloc'. Free the returned buffer with 'free' if there is no error.
Error ReadFileWithComments(FILE* err_out, const char* path, char** ptr);

// TODO: TOCTOU vulnerability?
bool DoesPathExists(const ObjectString* path);

#endif // LOOP_FILESYSTEM_H
