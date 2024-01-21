#ifndef LOOP_UTIL_H
#define LOOP_UTIL_H

#include "Common.h"

/// Uses 'malloc'. Free the returned buffer with 'free' if there is no error.
Error ReadFile(const char* path, char** ptr);
void ProcessReadError(Error self, const char* path, FILE* out);

// TODO: MERGE THESE FUINCTIONCS.

#endif // LOOP_UTIL_H
