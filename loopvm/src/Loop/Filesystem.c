#include "Filesystem.h"

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
