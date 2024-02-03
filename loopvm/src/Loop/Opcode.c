#include "Opcode.h"

const char *OpcodeToString(Opcode self) {
    switch (self) {
#define Opcode_TO_STRING(name, _) case Opcode_##name: return #name;

        Opcode_LIST(Opcode_TO_STRING);

#undef Opcode_TO_STRING

        default:
            return "Unknown";
    }
}
