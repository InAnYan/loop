#include "Util.h"

Error ReadFile(const char* path, char** ptr)
{
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        return Error_FileNotFound;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = malloc(file_size + 1);
    if (buffer == NULL)
    {
        return Error_OutOfMemory;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size)
    {
        free(buffer);
        return Error_IOError;
    }

    buffer[bytes_read] = '\0';
    fclose(file);
    *ptr = buffer;

    return Error_None;
}

void ProcessReadError(Error self, const char* path, FILE* out)
{
    if (self == Error_IOError)
    {
        fprintf(out, "error: cannot open file '%s'\n", path);
    }

    if (self == Error_InvalidJSON)
    {
        fprintf(out, "error: invalid JSON in file '%s'\n", path);
    }

    if (self == Error_OutOfMemory)
    {
        fprintf(out, "error: out of memory\n");
    }
}
