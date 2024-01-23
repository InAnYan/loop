#include <stdio.h>

#include "Loop/Object.h"
#include "Loop/Util.h"
#include "Loop/VirtualMachine.h"

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "error: wrong arguments count\n");
        fprintf(stderr, "usage: loopvm <path>\n");
        return Error_WrongArgumentsCount;
    }

    const char* path = argv[1];

    VirtualMachineConfiguration conf = {.user_out = stdout, .user_err = stderr, .debug_out = stderr, .user_in = stdin};
    VirtualMachine vm;
    VirtualMachineInit(&vm, conf);

    ObjectModule* module = NULL;
    Error err = VirtualMachineLoadModule(&vm, ObjectStringFromLiteral(&vm, path), &module);
    if (err != Error_None)
    {
        ProcessReadError(err, path, conf.user_err);
        VirtualMachineDeinit(&vm);
        return err;
    }

    Error error = VirtualMachineRunScript(&vm, module->script);

    VirtualMachineDeinit(&vm);

    #ifdef DEBUG_MODE
    // printf("%s\n", ErrorToString(error));
    #endif

    return error;
}
