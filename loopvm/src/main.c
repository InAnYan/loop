#include <stdio.h>

#include "Loop/Object.h"
#include "Loop/VirtualMachine.h"

#include "Loop/Objects/String.h"
#include "Loop/Objects/Module.h"

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "error: wrong arguments count\n");
        fprintf(stderr, "usage: loopvm <path>\n");
        return Error_WrongArgumentsCount;
    }

    const char* path = argv[1];

    VirtualMachine vm;
    {
        Error err = VirtualMachineInit(&vm);
        if (err != Error_None)
        {
            VirtualMachineDeinit(&vm);
            return err;
        }
    }

    ObjectModule* module = NULL;
    Error err = VirtualMachineLoadModule(&vm, vm.common.empty_string, ObjectStringFromLiteral(&vm, path), &module);
    if (err != Error_None)
    {
        VirtualMachineDeinit(&vm);
        return err;
    }

    Error error = VirtualMachineRunScript(&vm, module->script);

    VirtualMachineDeinit(&vm);

    #ifdef LOOP_DEBUG_MODE
    // printf("%s\n", ErrorToString(error));
    #endif

    return error;
}
