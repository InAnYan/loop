#include "ObjectType.h"

const char *ObjectTypeToString(ObjectType self) {
    switch (self) {
#define ObjectType_TO_STRING(name) case ObjectType_##name: return #name;

        ObjectType_LIST(ObjectType_TO_STRING);

#undef ObjectType_TO_STRING

        default:
            return "UnknownObjectType";
    }
}
