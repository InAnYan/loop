#include "Value.h"

#include "Object.h"

#include "Objects/String.h"

const char* ValueTypeToString(ValueType self)
{
    switch (self)
    {
        #define ValueType_TO_STRING(name) case ValueType_##name: return #name;

    ValueType_LIST(ValueType_TO_STRING);

        #undef ValueType_TO_STRING

    default:
        return "UnknownValueType";
    }
}

Value ValueFromJSON(VirtualMachine* vm, ObjectModule* module, const cJSON* json)
{
    assert(cJSON_IsObject(json));

    const cJSON* type = cJSON_GetObjectItemCaseSensitive(json, "type");
    assert(cJSON_IsString(type));

    const char* type_string = type->valuestring;

    if (strcmp(type_string, "Integer") == 0)
    {
        const cJSON* value = cJSON_GetObjectItemCaseSensitive(json, "data");
        assert(cJSON_IsNumber(value));

        return ValueInt(value->valueint);
    }

    return ValueObject(ObjectFromJSON(vm, module, json));
}

Value ValueNull()
{
    Value result;
    result.type = ValueType_Null;
    result.as.integer = 0;
    return result;
}

Value ValueBool(bool value)
{
    Value result;
    result.type = ValueType_Bool;
    result.as.boolean = value;
    return result;
}

Value ValueInt(int value)
{
    Value result;
    result.type = ValueType_Int;
    result.as.integer = value;
    return result;
}

Value ValueObject(Object* value)
{
    Value result;
    result.type = ValueType_Object;
    result.as.object = value;
    return result;
}

ValueType ValueGetType(Value self)
{
    return self.type;
}

bool ValueIsNull(Value self)
{
    return ValueGetType(self) == ValueType_Null;
}

bool ValueIsBool(Value self)
{
    return ValueGetType(self) == ValueType_Bool;
}

bool ValueIsInt(Value self)
{
    return ValueGetType(self) == ValueType_Int;
}

bool ValueIsObject(Value self)
{
    return ValueGetType(self) == ValueType_Object;
}

bool ValueAsBool(Value self)
{
    return self.as.boolean;
}

int ValueAsInt(Value self)
{
    return self.as.integer;
}

Object* ValueAsObject(Value self)
{
    return self.as.object;
}

void ValuePrint(Value self, FILE* out)
{
    switch (ValueGetType(self))
    {
    case ValueType_Null:
        fprintf(out, "null");
        break;
    case ValueType_Bool:
        fprintf(out, "%s", ValueAsBool(self) ? "true" : "false");
        break;
    case ValueType_Int:
        fprintf(out, "%d", ValueAsInt(self));
        break;
    case ValueType_Object:
        ObjectPrint(ValueAsObject(self), out);
        break;
    }
}

bool ValueAreEqual(Value a, Value b)
{
    if (ValueGetType(a) != ValueGetType(b))
    {
        return false;
    }

    switch (ValueGetType(a))
    {
    case ValueType_Null:
        return true;
    case ValueType_Bool:
        return ValueAsBool(a) == ValueAsBool(b);
    case ValueType_Int:
        return ValueAsInt(a) == ValueAsInt(b);
    case ValueType_Object:
        return ValueAsObject(a) == ValueAsObject(b);
    }
}

size_t ValueHash(Value self)
{
    switch (ValueGetType(self))
    {
    case ValueType_Null:
        assert(false && "Null cannot be hashed");
    case ValueType_Bool:
        return ValueAsBool(self) ? 1 : 0;
    case ValueType_Int:
        return ValueAsInt(self);
    case ValueType_Object:
    {
        Object* obj = ValueAsObject(self);
        if (!ObjectIsString(obj))
        {
            assert(false && "Only strings can be hashed");
        }

        return ObjectAsString(obj)->hash;
    }
    }
}

bool ValueIsTrue(Value self)
{
    return !ValueIsFalse(self);
}

bool ValueIsFalse(Value self)
{
    switch (ValueGetType(self))
    {
    case ValueType_Null:
        return true;
    case ValueType_Bool:
        return !ValueAsBool(self);
    default:
        return false;
    }
}
