#include "Error.h"

const char *ErrorToString(Error self) {
    switch (self) {
#define Error_TO_STRING(name) case Error_##name: return #name;

        Error_LIST(Error_TO_STRING);

#undef Error_TO_STRING

        default:
            return "Unknown";
    }
}
