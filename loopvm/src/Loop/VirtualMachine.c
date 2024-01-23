#include "VirtualMachine.h"

#include "Object.h"
#include "Opcode.h"
#include "Util.h"

void CommonStringsInit(CommonStrings* self, VirtualMachine* vm)
{
    self->script = ObjectStringFromLiteral(vm, "script");
}

void VirtualMachineInit(VirtualMachine* self, VirtualMachineConfiguration conf)
{
    self->conf = conf;
    MemoryManagerInit(&self->memory_manager, self); // Potential bug, if conf is not set in VM.
    self->stack_ptr = self->stack;
    self->frame_ptr = self->frames;
    HashTableInit(&self->strings);
    HashTableInitWithCapacity(&self->modules, self);

    CommonStringsInit(&self->common_strings, self); // Bug if a lot is not set.
}

void VirtualMachineDeinit(VirtualMachine* self)
{
    HashTableDeinit(&self->strings, self);
    HashTableDeinit(&self->modules, self);
    MemoryManagerDeinit(&self->memory_manager);
}

static bool PushScript(VirtualMachine* self, ObjectFunction* function);
static bool PushFrame(VirtualMachine* self, ObjectFunction* function);
static bool PopFrame(VirtualMachine* self);

static Value StackPeek(VirtualMachine* self);
static Value StackPeekAt(VirtualMachine* self, size_t offset);
static Value StackPeekLocal(VirtualMachine* self, size_t offset);
static Value StackPop(VirtualMachine* self);
static void StackPush(VirtualMachine* self, Value value);

static HashTable* GetGlobals(CallFrame* frame);
static HashTable* GetExports(CallFrame* frame);

static uint8_t ReadByte(CallFrame* frame);
static uint16_t ReadShort(CallFrame* frame);
static Value ReadConstant(CallFrame* frame);

static void TraceStack(VirtualMachine* self);

typedef enum BinaryOp
{
    BinaryOp_Add,
    BinaryOp_Subtract,
    BinaryOp_Multiply,
    BinaryOp_Divide,
    BinaryOp_Greater,
    BinaryOp_Less,
} BinaryOp;

static Error BinOp(VirtualMachine* self, BinaryOp op);
static Error Call(VirtualMachine* self, Value value, uint8_t arity);
static Error GetItem(VirtualMachine* self, Value value, uint8_t arity);
static Error SetItem(VirtualMachine* self, Value value, uint8_t arity);
static Error GetAttribute(VirtualMachine* self, Value value, Value key);

static Error Run(VirtualMachine* self);

Error VirtualMachineRunScript(VirtualMachine* self, ObjectFunction* script)
{
    bool res = PushScript(self, script);
    assert(res);
    return Run(self);
}

Error VirtualMachineLoadModule(VirtualMachine* self, ObjectString* path, ObjectModule** ptr)
{
    Value interned;
    if (HashTableGet(&self->modules, ValueObject((Object*)path), &interned))
    {
        assert(ValueIsObject(interned) && ObjectIsModule(ValueAsObject(interned)));
        *ptr = ObjectAsModule(ValueAsObject(interned));
        return Error_None;
    }

    char* buffer = NULL;
    Error error = ReadFile(path->str, &buffer);
    if (error != Error_None)
    {
        return error;
    }

    const cJSON* data = cJSON_Parse(buffer);
    if (data == NULL)
    {
        return Error_InvalidJSON;
    }

    free(buffer);

    *ptr = ObjectModuleFromJSON(self, NULL, data);

    return Error_None;
}

// TODO: Check value type object types and classes.
#define CHECK_VALUE_TYPE(self, value, type) \
    do \
    { \
        if (!ValueIs##type(value)) \
        { \
            fprintf(self->conf.user_err, "error: expected %s, got %s\n", \
                #type, ValueTypeToString(ValueGetType(value))); \
            return Error_TypeMismatch; \
        } \
    } while (0)

Error Run(VirtualMachine* self)
{
    while (true)
    {
        CallFrame* frame = &self->frame_ptr[-1];

        #ifdef VM_TRACE_EXECUTION

        TraceStack(self);
        ChunkDisassembleInstruction(&frame->function->chunk, self->conf.debug_out, frame->ip);

        #endif

        Opcode opcode = ReadByte(frame);

        switch (opcode)
        {
        case Opcode_PushConstant:
        {
            Value value = ReadConstant(frame);
            StackPush(self, value);
            break;
        }

        case Opcode_PushFalse:
        {
            StackPush(self, ValueBool(false));
            break;
        }

        case Opcode_PushTrue:
        {
            StackPush(self, ValueBool(true));
            break;
        }

        case Opcode_PushNull:
        {
            StackPush(self, ValueNull());
            break;
        }

        case Opcode_Negate:
        {
            Value value = StackPop(self);
            CHECK_VALUE_TYPE(self, value, Int);
            StackPush(self, ValueInt(-ValueAsInt(value)));
            break;
        }

        case Opcode_Not:
        {
            Value value = StackPop(self);
            StackPush(self, ValueBool(ValueIsFalse(value)));
            break;
        }

        case Opcode_Plus:
        {
            break;
        }

            #define BIN_OP(self, op) \
                case Opcode_##op: \
                    { \
                        Error error = BinOp(self, BinaryOp_##op); \
                        if (error != Error_None) \
                        { \
                             return error; \
                        } \
                        break; \
                    }

        BIN_OP(self, Add);
        BIN_OP(self, Subtract);
        BIN_OP(self, Multiply);
        BIN_OP(self, Divide);
        BIN_OP(self, Greater);
        BIN_OP(self, Less);

        case Opcode_Equal:
        {
            // TODO: Objects custom equality.
            Value b = StackPop(self);
            Value a = StackPop(self);
            StackPush(self, ValueBool(ValueAreEqual(a, b)));
            break;
        }

        // Code duplication...

            #define JUMP_COND(self, frame, opcode, op, bool, mode) \
                do \
                { \
                    uint16_t offset = ReadShort(frame); \
                    if (ValueIs##bool(Stack##mode(self))) \
                    { \
                        frame->ip op##= offset; \
                    } \
                } while (false)

        case Opcode_JumpIfFalse:
            JUMP_COND(self, frame, opcode, +, False, Peek);
            break;

        case Opcode_JumpIfFalsePop:
            JUMP_COND(self, frame, opcode, +, False, Pop);
            break;

        case Opcode_JumpIfTrue:
            JUMP_COND(self, frame, opcode, +, True, Peek);
            break;

            #define JUMP_UNCOND(frame, op) \
                do \
                { \
                    uint16_t offset = ReadShort(frame); \
                    frame->ip op##= offset; \
                } while (false)

        case Opcode_Jump:
            JUMP_UNCOND(frame, +);
            break;

        case Opcode_Loop:
            JUMP_UNCOND(frame, -);
            break;

        case Opcode_Print:
        {
            // TODO: Objects custom printing.
            Value value = StackPop(self);
            ValuePrint(value, self->conf.user_out, PrintFlags_Pretty);
            fprintf(self->conf.user_out, "\n");
            break;
        }

        case Opcode_Pop:
        {
            StackPop(self);
            break;
        }

        case Opcode_DefineGlobal:
        {
            Value value = StackPop(self);
            Value key = ReadConstant(frame); // The compiler is responsible for correctness.

            if (!HashTablePut(GetGlobals(frame), self, key, value))
            {
                fprintf(self->conf.user_err,
                        "error: variable redefinition: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_VariableRedefinition;
            }

            break;
        }

        case Opcode_GetGlobal:
        {
            Value key = ReadConstant(frame);

            Value value;
            if (!HashTableGet(GetGlobals(frame), key, &value))
            {
                fprintf(self->conf.user_err,
                        "error: undefined variable: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_UndefinedVariable;
            }

            StackPush(self, value);

            break;
        }

        case Opcode_SetGlobal:
        {
            Value key = ReadConstant(frame);
            Value value = StackPeek(self);

            if (HashTablePut(GetGlobals(frame), self, key, value))
            {
                fprintf(self->conf.user_err,
                        "error: undefined variable: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_UndefinedVariable;
            }

            break;
        }

        case Opcode_GetLocal:
        {
            uint8_t slot = ReadByte(frame);
            StackPush(self, frame->locals[slot]);
            break;
        }

        case Opcode_SetLocal:
        {
            uint8_t slot = ReadByte(frame);
            frame->locals[slot] = StackPeek(self);
            break;
        }

            #define CALL_LIKE_OP(self, frame, op) \
                case Opcode_##op: \
                    { \
                        uint8_t arg_count = ReadByte(frame); \
                        Value function = StackPeekAt(self, arg_count); \
                        \
                        Error error = op(self, function, arg_count); \
                        if (error != Error_None) \
                        { \
                            return error; \
                        } \
                        break; \
                    }

        CALL_LIKE_OP(self, frame, Call);
        CALL_LIKE_OP(self, frame, GetItem);
        CALL_LIKE_OP(self, frame, SetItem);

        case Opcode_Return:
        {
            // TODO: uhm, probably bug with the first script that is very last at the end.

            Value value = StackPop(self);

            if (!PopFrame(self))
            {
                return Error_StackUnderflow;
            }

            if (self->frame_ptr == self->frames)
            {
                return Error_None;
            }

            StackPush(self, value);

            break;
        }

        case Opcode_Export:
        {
            Value value = StackPop(self);
            Value key = ReadConstant(frame);

            if (!HashTablePut(GetGlobals(frame), self, key, value))
            {
                fprintf(self->conf.user_err,
                        "error: variable redefinition: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_VariableRedefinition;
            }

            if (!HashTablePut(GetExports(frame), self, key, value))
            {
                fprintf(self->conf.user_err,
                        "error: variable reexport: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_VariableRedefinition;
            }

            break;
        }

        case Opcode_Import:
        {
            // TODO: BUG in compiler first statement is import wrong line number.
            // TODO: Or maybe there.

            Value key = ReadConstant(frame);
            ObjectString* str = ObjectAsString(ValueAsObject(key));

            ObjectModule* module;
            Error error = VirtualMachineLoadModule(self, str, &module);
            if (error != Error_None)
            {
                return error;
            }

            if (module->is_partial && !PushScript(self, module->script))
            {
                return Error_StackOverflow;
            }

            break;
        }

        case Opcode_Top:
        {
            StackPush(self, StackPeek(self));
            break;
        }

        case Opcode_GetAttribute:
        {
            Value value = StackPop(self);
            Value key = ReadConstant(frame);

            Error error = GetAttribute(self, value, key);
            if (error != Error_None)
            {
                return error;
            }

            break;
        }

        case Opcode_ModuleEnd:
        {
            // Code duplication(
            // Please forgive me.

            StackPop(self);

            frame->function->module->is_partial = false;
            Value module = ValueObject((Object*)frame->function->module);

            if (!PopFrame(self))
            {
                return Error_StackUnderflow;
            }

            if (self->frame_ptr == self->frames)
            {
                return Error_None;
            }

            StackPush(self, module);

            break;
        }

        case Opcode_BuildDictionary:
        {
            uint8_t count = ReadByte(frame);

            ObjectDictionary* obj = ObjectDictionaryNew(self);

            for (int i = 0; i < count; ++i)
            {
                Value value = StackPop(self);
                Value key = StackPop(self);
                HashTablePut(&obj->entries, self, key, value);
            }

            StackPush(self, ValueObject((Object*)obj));

            break;
        }

        default:
            fprintf(self->conf.user_err, "FATAL ERROR: unknown opcode: 0x%02x\n", opcode);
            return Error_UnknownOpcode;
        }
    }
}

static bool PushScript(VirtualMachine* self, ObjectFunction* script)
{
    StackPush(self, ValueObject((Object*)script));

    return PushFrame(self, script);
}

static bool PushFrame(VirtualMachine* self, ObjectFunction* function)
{
    if (self->frame_ptr - self->frames == VM_FRAMES_COUNT)
    {
        return false;
    }

    CallFrame* frame = self->frame_ptr++;
    frame->function = function;
    frame->ip = function->chunk.code;
    frame->locals = self->stack_ptr - function->arity - 1;

    return true;
}

static bool PopFrame(VirtualMachine* self)
{
    if (self->frame_ptr == self->frames)
    {
        return false;
    }

    --self->frame_ptr;
    self->stack_ptr = self->frame_ptr->locals;

    return true;
}

static Value StackPeek(VirtualMachine* self)
{
    assert(self->stack_ptr != self->stack);
    return self->stack_ptr[-1];
}

static Value StackPeekAt(VirtualMachine* self, size_t offset)
{
    assert(offset < self->stack_ptr - self->stack);
    return self->stack_ptr[-1 - offset];
}

static Value StackPeekLocal(VirtualMachine* self, size_t offset)
{
    assert(self->frame_ptr != self->frames);
    assert(offset < self->frame_ptr->locals - self->stack_ptr);
    return self->frame_ptr[-1].locals[offset];
}

static Value StackPop(VirtualMachine* self)
{
    assert(self->stack_ptr != self->stack);
    return *--self->stack_ptr;
}

static void StackPush(VirtualMachine* self, Value value)
{
    assert(self->stack_ptr != self->stack + VM_STACK_SIZE_PER_FRAME * VM_FRAMES_COUNT + 1);
    *self->stack_ptr++ = value;
}

static HashTable* GetGlobals(CallFrame* frame)
{
    // TODO: Cache globals.
    return &frame->function->module->globals;
}

static HashTable* GetExports(CallFrame* frame)
{
    return &frame->function->module->exports;
}

static uint8_t ReadByte(CallFrame* frame)
{
    assert(frame->ip != frame->function->chunk.code + frame->function->chunk.code_length);
    return *(frame->ip++);
}

static uint16_t ReadShort(CallFrame* frame)
{
    uint16_t part_1 = ReadByte(frame);
    uint16_t part_2 = ReadByte(frame);
    return (part_2 << 8) | part_1;
}

static Value ReadConstant(CallFrame* frame)
{
    uint8_t index = ReadByte(frame);
    assert(index < frame->function->chunk.constants_length);
    return frame->function->chunk.constants[index];
}

static void TraceStack(VirtualMachine* self)
{
    FILE* out = self->conf.debug_out;

    for (Value* ptr = self->stack; ptr != self->stack_ptr; ++ptr)
    {
        fprintf(out, "[ ");
        ValuePrint(*ptr, out, PrintFlags_Debug);
        fprintf(out, " ]");
    }

    fprintf(out, "\n");
}

static Error BinOp(VirtualMachine* self, BinaryOp op)
{
    Value b = StackPop(self);
    Value a = StackPop(self);

    CHECK_VALUE_TYPE(self, a, Int);
    CHECK_VALUE_TYPE(self, b, Int);

    int rhs = ValueAsInt(b);
    int lhs = ValueAsInt(a);

    switch (op)
    {
    case BinaryOp_Add:
        StackPush(self, ValueInt(lhs + rhs));
        break;
    case BinaryOp_Subtract:
        StackPush(self, ValueInt(lhs - rhs));
        break;
    case BinaryOp_Multiply:
        StackPush(self, ValueInt(lhs * rhs));
        break;
    case BinaryOp_Divide:
        if (rhs == 0)
        {
            fprintf(self->conf.user_err, "error: zero division\n");
            return Error_ZeroDivision;
        }
        StackPush(self, ValueInt(lhs / rhs));
        break;
    case BinaryOp_Greater:
        StackPush(self, ValueBool(lhs > rhs));
        break;
    case BinaryOp_Less:
        StackPush(self, ValueBool(lhs < rhs));
        break;
    }

    return Error_None;
}

static Error Call(VirtualMachine* self, Value value, uint8_t arity)
{
    if (!ValueIsObject(value))
    {
        fprintf(self->conf.user_err, "error: expected callable, got %s\n",
                ValueTypeToString(ValueGetType(value)));
        return Error_NonCallable;
    }

    Object* obj = ValueAsObject(value);
    ObjectType obj_type = ObjectGetType(obj);

    switch (obj_type)
    {
    case ObjectType_Function:
    {
        ObjectFunction* func = ObjectAsFunction(obj);

        if (func->arity != arity)
        {
            fprintf(self->conf.user_err, "error: wrong number of arguments, expected %ld, got %d\n",
                    func->arity, arity);
            return Error_WrongArgumentsCount;
        }

        if (!PushFrame(self, func))
        {
            return Error_StackOverflow;
        }

        return Error_None;
    }

    default:
        fprintf(self->conf.user_err, "error: expected callable, got %s\n",
                ObjectTypeToString(obj_type));
        return Error_NonCallable;
    }
}

static Error GetObjectAttribute(VirtualMachine* self, Object* obj, Value key);

static Error GetAttribute(VirtualMachine* self, Value value, Value key)
{
    switch (ValueGetType(value))
    {
    case ValueType_Object:
        return GetObjectAttribute(self, ValueAsObject(value), key);
    default:
        fprintf(self->conf.user_err, "error: cannot get attribute from %s\n", ValueTypeToString(ValueGetType(value)));
        return Error_TypeMismatch;
    }
}

static Error GetObjectAttribute(VirtualMachine* self, Object* obj, Value key)
{
    switch (ObjectGetType(obj))
    {
    case ObjectType_Module:
    {
        ObjectModule* module = ObjectAsModule(obj);

        Value value;
        if (!HashTableGet(&module->exports, key, &value))
        {
            fprintf(self->conf.user_err, "error: undefined export: '%s'\n", ObjectAsString(ValueAsObject(key))->str);
            return Error_UndefinedExport;
        }

        StackPush(self, value);
        break;
    }

    default:
        fprintf(self->conf.user_err, "error: cannot get attribute from %s\n", ObjectTypeToString(ObjectGetType(obj)));
        return Error_TypeMismatch;
    }

    return Error_None;
}

static Error GetItem(VirtualMachine* self, Value value, uint8_t arity)
{
    CHECK_VALUE_TYPE(self, value, Object);

    // TODO: This arity check is not general.
    if (arity != 1)
    {
        fprintf(self->conf.user_err, "error: wrong number of arguments, expected 1, got %d\n",
                arity);
        return Error_WrongArgumentsCount;
    }

    Value arg = StackPop(self);
    StackPop(self); // value

    Object* obj = ValueAsObject(value);

    switch (ObjectGetType(obj))
    {
    case ObjectType_String:
    {
        ObjectString* str = ObjectAsString(obj);
        // TODO: CHARACTER TYPE?

        CHECK_VALUE_TYPE(self, arg, Int);

        int index = ValueAsInt(arg);
        if (index < 0 || index >= str->length)
        {
            fprintf(self->conf.user_err, "error: index out of range\n");
            return Error_OutOfRange;
        }

        StackPush(self, ValueInt(str->str[index]));

        break;
    }

    case ObjectType_Dictionary:
    {
        ObjectDictionary* dictionary = ObjectAsDictionary(obj);

        Value result;
        if (!HashTableGet(&dictionary->entries, arg, &result))
        {
            fprintf(self->conf.user_err, "error: undefined key: ");
            ValuePrint(arg, self->conf.user_err, PrintFlags_Debug);
            return Error_OutOfRange;
        }

        StackPush(self, result);
        break;
    }

    default:
        fprintf(self->conf.user_err, "error: cannot get item from %s\n",
                ObjectTypeToString(ObjectGetType(obj)));
        return Error_TypeMismatch;
    }

    return Error_None;
}

static Error SetItem(VirtualMachine* self, Value value, uint8_t arity)
{
    CHECK_VALUE_TYPE(self, value, Object);

    // TODO: This arity check is not general.
    if (arity != 2)
    {
        fprintf(self->conf.user_err, "error: wrong number of arguments, expected 1, got %d\n",
                arity);
        return Error_WrongArgumentsCount;
    }

    Value assign = StackPop(self);
    Value arg = StackPop(self);

    Object* obj = ValueAsObject(value);

    // TODO: RESULT OF ASSIGNEMNT OF AN ITEM?
    // Currently it is the original object.

    switch (ObjectGetType(obj))
    {
    case ObjectType_String:
    {
        ObjectString* str = ObjectAsString(obj);
        // TODO: CHARACTER TYPE?

        CHECK_VALUE_TYPE(self, arg, Int);
        CHECK_VALUE_TYPE(self, assign, Int);

        int index = ValueAsInt(arg);
        if (index < 0 || index >= str->length)
        {
            fprintf(self->conf.user_err, "error: index out of range\n");
            return Error_OutOfRange;
        }

        // TODO: Modifiyable strings - yes or no?
        str->str[index] = ValueAsInt(assign);

        break;
    }

    case ObjectType_Dictionary:
    {
        ObjectDictionary* dictionary = ObjectAsDictionary(obj);

        HashTablePut(&dictionary->entries, self, arg, assign);
        break;
    }

    default:
        fprintf(self->conf.user_err, "error: cannot set item of %s\n",
                ObjectTypeToString(ObjectGetType(obj)));
        return Error_TypeMismatch;
    }

    return Error_None;
}
