#include <stdio.h>

#include "Loop/Object.h"
#include "Loop/VirtualMachine.h"

Error ReadFile(const char* path, FILE* err_out, const char** ptr);

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "error: wrong arguments count\n");
        fprintf(stderr, "usage: loopvm <path>\n");
        return Error_WrongArgumentsCount;
    }

    const char* path = argv[1];

    const char* buffer;

    Error read_error = ReadFile(path, stderr, &buffer);
    if (read_error != Error_None)
    {
        return read_error;
    }

    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL)
    {
        free((void*)buffer);
        return Error_IOError;
    }

    VirtualMachineConfiguration conf = {.user_out = stdout, .user_err = stderr, .debug_out = stderr, .user_in = stdin};
    VirtualMachine vm;
    VirtualMachineInit(&vm, conf);
    ObjectFunction* script = ObjectFunctionFromJSON(&vm, json);
    Value res = VirtualMachineRunScript(&vm, script);
    VirtualMachineDeinit(&vm);

    if (ValueIsInt(res))
    {
        return ValueAsInt(res);
    }

    return 0;
}

Error ReadFile(const char* path, FILE* err_out, const char** ptr)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(err_out, "error: could not open file \"%s\"\n", path);
        return Error_IOError;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = malloc(file_size + 1);
    if (buffer == NULL)
    {
        fprintf(err_out, "error: could not allocate memory for file \"%s\"\n", path);
        return Error_OutOfMemory;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size)
    {
        fprintf(err_out, "error: could not read file \"%s\"\n", path);
        return Error_IOError;
    }

    buffer[bytes_read] = '\0';
    fclose(file);
    *ptr = buffer;

    return Error_None;
}
