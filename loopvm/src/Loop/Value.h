#ifndef LOOP_VALUE_H
#define LOOP_VALUE_H

#include "Common.h"

// TODO: YOU CANNOT USE NULL TO INDEX DICTIONARIES.

#define ValueType_LIST(o) \
    o(Null) \
    o(Bool) \
    o(Int) \
    o(Object)

typedef enum ValueType
{
    #define ValueType_ENUM(name) ValueType_##name,

    ValueType_LIST(ValueType_ENUM)

    #undef ValueType_ENUM
} ValueType;

const char* ValueTypeToString(ValueType value);

typedef union ValueUnion
{
    bool boolean;
    int integer;
    Object* object;
} ValueUnion;

typedef struct Value
{
    ValueType type;
    ValueUnion as;
} Value;

/// Use this function with caution. module_path will be set to NULL.
Value ValueFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* json);

Value ValueNull();
Value ValueBool(bool value);
Value ValueInt(int value);
Value ValueObject(Object* value);

ValueType ValueGetType(Value self);

bool ValueIsNull(Value self);
bool ValueIsBool(Value self);
bool ValueIsInt(Value self);
bool ValueIsObject(Value self);

bool ValueAsBool(Value self);
int ValueAsInt(Value self);
Object* ValueAsObject(Value self);

void ValuePrint(Value self, FILE* out);
bool ValueAreEqual(Value a, Value b);
size_t ValueHash(Value self);

bool ValueIsTrue(Value self);
bool ValueIsFalse(Value self);

#endif // LOOP_VALUE_H
