#include "VirtualMachine.h"

#include "Filesystem.h"
#include "Object.h"
#include "Opcode.h"

#include "Objects/BoundMethod.h"
#include "Objects/Class.h"
#include "Objects/String.h"
#include "Objects/Dictionary.h"
#include "Objects/Function.h"
#include "Objects/Instance.h"
#include "Objects/Module.h"

void CommonObjectsInit(CommonObjects* self, VirtualMachine* vm)
{
    self->script = ObjectStringFromLiteral(vm, "script");
    self->init = ObjectStringFromLiteral(vm, "init");
    self->empty_string = ObjectStringFromLiteral(vm, "");
}

void CommonObjectsDeinit(CommonObjects* self)
{
    self->empty_string = NULL;
    self->init = NULL;
    self->script = NULL;
}

Error VirtualMachineInit(VirtualMachine* self)
{
    MemoryManagerInit(&self->memory_manager, self); // Potential bug, if conf is not set in VM.
    self->stack_ptr = self->stack;
    self->frame_ptr = self->frames;
    HashTableInit(&self->strings);
    HashTableInitWithCapacity(&self->modules, self);
    CommonObjectsInit(&self->common, self); // Bug if a lot is not set.

    self->called_path = GetCurrentWorkingDirectory(self);
    if (self->called_path == NULL)
    {
        fprintf(USER_ERR, "FATAL ERROR: failed to get current working directory.\n");
        return Error_IOError;
    }

    const char* packages_path = getenv("LOOP_PACKAGES_PATH");
    if (packages_path == NULL)
    {
        fprintf(USER_ERR, "FATAL ERROR: LOOP_PACKAGES_PATH is not set.\n");
        return Error_IOError;
    }

    self->packages_path = ObjectStringFromLiteral(self, packages_path);

    return Error_None;
}

void VirtualMachineDeinit(VirtualMachine* self)
{
    self->packages_path = NULL;
    self->called_path = NULL;
    HashTableDeinit(&self->modules, self);
    HashTableDeinit(&self->strings, self);
    self->frame_ptr = NULL;
    self->stack_ptr = NULL;
    CommonObjectsDeinit(&self->common);
    MemoryManagerDeinit(&self->memory_manager);
}

static Error PushScript(VirtualMachine* self, ObjectFunction* function);
static Error PushFrame(VirtualMachine* self, ObjectFunction* function);
static Error PopFrame(VirtualMachine* self);

static Value StackPeek(VirtualMachine* self);
static Value StackPeekAt(VirtualMachine* self, size_t offset);
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
static Error SetAttribute(VirtualMachine* self, Value value, Value key, Value instance);

static Error Run(VirtualMachine* self);

Error VirtualMachineRunScript(VirtualMachine* self, ObjectFunction* script)
{
    TRY(PushScript(self, script));
    return Run(self);
}

static bool InternModule(VirtualMachine* self, ObjectString* path, ObjectModule** ptr);
static Error LoadNewModule(VirtualMachine* self, ObjectString* path, ObjectModule** ptr);

Error VirtualMachineLoadModule(VirtualMachine* self, ObjectString* parent, ObjectString* path, ObjectModule** ptr)
{
    ObjectString* parent_paths[] = {parent, self->called_path, self->packages_path};
    ObjectString* constructed_paths[sizeof(parent_paths) / sizeof(parent_paths[0])] = {};

    for (size_t i = 0; i < sizeof(parent_paths) / sizeof(parent_paths[0]); ++i)
    {
        ObjectString* parent_path = parent_paths[i];
        ObjectString* combined_path = JoinPath(self, parent_path, path);
        ObjectString* abs_path = GetAbsolutePath(self, combined_path);

        constructed_paths[i] = abs_path;

        if (abs_path == NULL)
        {
            continue;
        }

        if (InternModule(self, abs_path, ptr))
        {
            return Error_None;
        }
    }

    for (size_t i = 0; i < sizeof(constructed_paths) / sizeof(constructed_paths[0]); ++i)
    {
        ObjectString* the_path = constructed_paths[i];

        if (the_path == NULL)
        {
            continue;
        }

        if (DoesPathExists(the_path))
        {
            return LoadNewModule(self, the_path, ptr);
        }
    }

    // TODO: This error print shows .code extension.
    fprintf(USER_ERR, "error: module '%s' not found.\n", path->str);
    return Error_FileNotFound;
}

static bool InternModule(VirtualMachine* self, ObjectString* path, ObjectModule** ptr)
{
    Value interned;
    if (!HashTableGet(&self->modules, ValueObject((Object*)path), &interned))
    {
        return false;
    }

    assert(ValueIsObject(interned) && ObjectIsModule(ValueAsObject(interned)));
    *ptr = ObjectAsModule(ValueAsObject(interned));
    return true;
}

static Error LoadNewModule(VirtualMachine* self, ObjectString* path, ObjectModule** ptr)
{
    char* buffer = NULL;
    TRY(ReadFileWithComments(USER_ERR, path->str, &buffer));

    cJSON* data = cJSON_Parse(buffer);
    if (data == NULL)
    {
        fprintf(USER_ERR, "error: failed to parse JSON for '%s'.\n",
                path->str);
        return Error_InvalidJSON;
    }

    free(buffer);

    *ptr = ObjectModuleFromJSON(self, path, data);

    cJSON_Delete(data);

    return Error_None;
}

// TODO: Check value type object types and classes.
#define CHECK_VALUE_TYPE(self, value, type) \
    do \
    { \
        if (!ValueIs##type(value)) \
        { \
            fprintf(USER_ERR, "error: expected %s, got %s\n", \
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
        ChunkDisassembleInstruction(&frame->function->chunk, DEBUG_OUT, frame->ip);

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
                    TRY(BinOp(self, BinaryOp_##op)); \
                    break;
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
            ValuePrint(value, USER_OUT);
            fprintf(USER_OUT, "\n");
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
                fprintf(USER_ERR,
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
                fprintf(USER_ERR,
                        "error: undefined variable: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_UndefinedReference;
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
                fprintf(USER_ERR,
                        "error: undefined variable: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_UndefinedReference;
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
                        TRY(op(self, function, arg_count)); \
                        break; \
                    }

        CALL_LIKE_OP(self, frame, Call);
        CALL_LIKE_OP(self, frame, GetItem);
        CALL_LIKE_OP(self, frame, SetItem);

        case Opcode_Return:
        {
            // TODO: uhm, probably bug with the first script that is very last at the end.

            Value value = StackPop(self);

            TRY(PopFrame(self));

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
                fprintf(USER_ERR,
                        "error: variable redefinition: '%s'\n",
                        ObjectAsString(ValueAsObject(key))->str);
                return Error_VariableRedefinition;
            }

            if (!HashTablePut(GetExports(frame), self, key, value))
            {
                fprintf(USER_ERR,
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

            ObjectModule* module = NULL;
            TRY(VirtualMachineLoadModule(self, frame->function->module->parent_dir, str, &module));

            if (module->is_partial)
            {
                TRY(PushScript(self, module->script));
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
            Value key = ReadConstant(frame);
            Value value = StackPop(self);

            TRY(GetAttribute(self, value, key));

            break;
        }

        case Opcode_SetAttribute:
        {
            Value key = ReadConstant(frame);
            Value value = StackPop(self);
            Value instance = StackPop(self);

            TRY(SetAttribute(self, value, key, instance));

            break;
        }

        case Opcode_ModuleEnd:
        {
            // Code duplication(
            // Please forgive me.

            StackPop(self);

            frame->function->module->is_partial = false;
            Value module = ValueObject((Object*)frame->function->module);

            TRY(PopFrame(self));

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
            fprintf(USER_ERR, "FATAL ERROR: unknown opcode: 0x%02x\n", opcode);
            return Error_UnknownOpcode;
        }
    }
}

static Error PushScript(VirtualMachine* self, ObjectFunction* script)
{
    StackPush(self, ValueObject((Object*)script));

    return PushFrame(self, script);
}

static Error PushFrame(VirtualMachine* self, ObjectFunction* function)
{
    if (self->frame_ptr - self->frames == VM_FRAMES_COUNT)
    {
        // TODO: ERROR?
        return Error_StackOverflow;
    }

    CallFrame* frame = self->frame_ptr++;
    frame->function = function;
    frame->ip = function->chunk.code;
    frame->locals = self->stack_ptr - function->arity - 1;

    return Error_None;
}

static Error PopFrame(VirtualMachine* self)
{
    if (self->frame_ptr == self->frames)
    {
        // TODO: ERROR?
        return Error_StackUnderflow;
    }

    --self->frame_ptr;
    self->stack_ptr = self->frame_ptr->locals;

    return Error_None;
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
    FILE* out = DEBUG_OUT;

    for (Value* ptr = self->stack; ptr != self->stack_ptr; ++ptr)
    {
        fprintf(out, "[ ");
        ValuePrint(*ptr, out);
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
            fprintf(USER_ERR, "error: zero division\n");
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
        fprintf(USER_ERR, "error: expected callable, got %s\n",
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
            fprintf(USER_ERR, "error: wrong number of arguments, expected %ld, got %d\n",
                    func->arity, arity);
            return Error_WrongArgumentsCount;
        }

        TRY(PushFrame(self, func));

        break;
    }

    case ObjectType_Class:
    {
        ObjectClass* klass = ObjectAsClass(obj);
        ObjectInstance* instance = ObjectInstanceNew(self, klass);

        self->stack_ptr[-arity - 1] = ValueObject((Object*)instance);

        Value init;
        if (HashTableGet(&klass->methods, ValueObject((Object*)self->common.init), &init))
        {
            return Call(self, init, arity);
        }

        if (arity != 0)
        {
            fprintf(USER_ERR, "error: wrong number of arguments, expected 0, got %d\n",
                    arity);
            return Error_WrongArgumentsCount;
        }

        break;
    }

    case ObjectType_BoundMethod:
    {
        ObjectBoundMethod* bound = ObjectAsBoundMethod(obj);
        self->stack_ptr[-arity - 1] = ValueObject((Object*)bound->receiver);
        return Call(self, ValueObject((Object*)bound->method), arity);
    }

    default:
        fprintf(USER_ERR, "error: expected callable, got %s\n",
                ObjectTypeToString(obj_type));
        return Error_NonCallable;
    }

    return Error_None;
}

static Error GetObjectAttribute(VirtualMachine* self, Object* obj, Value key);

static Error GetAttribute(VirtualMachine* self, Value value, Value key)
{
    switch (ValueGetType(value))
    {
    case ValueType_Object:
        return GetObjectAttribute(self, ValueAsObject(value), key);
    default:
        fprintf(USER_ERR, "error: cannot get attribute from %s\n", ValueTypeToString(ValueGetType(value)));
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
        if (HashTableGet(&module->exports, key, &value))
        {
            StackPush(self, value);
            return Error_None;
        }

        fprintf(USER_ERR, "error: undefined export: '%s'\n", ObjectAsString(ValueAsObject(key))->str);
        return Error_UndefinedReference;
    }

    case ObjectType_Instance:
    {
        ObjectInstance* instance = ObjectAsInstance(obj);
        Value value;

        if (HashTableGet(&instance->fields, key, &value))
        {
            StackPush(self, value);
            return Error_None;
        }

        if (HashTableGet(&instance->klass->methods, key, &value))
        {
            assert(ObjectIsFunction(ValueAsObject(value)));
            ObjectBoundMethod* bound = ObjectBoundMethodNew(self,
                                                            instance,
                                                            ObjectAsFunction(ValueAsObject(value)));
            StackPush(self, ValueObject((Object*)bound));
            return Error_None;
        }

        fprintf(USER_ERR, "error: undefined attribute: '%s'\n", ObjectAsString(ValueAsObject(key))->str);
        return Error_UndefinedReference;
    }

    default:
        fprintf(USER_ERR, "error: cannot get attribute from %s\n", ObjectTypeToString(ObjectGetType(obj)));
        return Error_TypeMismatch;
    }
}

static Error SetObjectAttribute(VirtualMachine* self, Value value, Value key, Object* instance);

static Error SetAttribute(VirtualMachine* self, Value value, Value key, Value instance)
{
    switch (ValueGetType(instance))
    {
    case ValueType_Object:
        return SetObjectAttribute(self, value, key, ValueAsObject(instance));
    default:
        fprintf(USER_ERR, "error: cannot set attribute for %s\n", ValueTypeToString(ValueGetType(instance)));
        return Error_TypeMismatch;
    }
}

static Error SetObjectAttribute(VirtualMachine* self, Value value, Value key, Object* instance)
{
    if (!ObjectIsInstance(instance))
    {
        fprintf(USER_ERR, "error: expected Instance, got %s\n",
                ObjectTypeToString(ObjectGetType(instance)));
        return Error_TypeMismatch;
    }

    ObjectInstance* obj = ObjectAsInstance(instance);

    HashTablePut(&obj->fields, self, key, value);
    StackPush(self, value);

    return Error_None;
}

static Error GetItem(VirtualMachine* self, Value value, uint8_t arity)
{
    CHECK_VALUE_TYPE(self, value, Object);

    // TODO: This arity check is not general.
    if (arity != 1)
    {
        fprintf(USER_ERR, "error: wrong number of arguments, expected 1, got %d\n",
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
            fprintf(USER_ERR, "error: index out of range\n");
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
            fprintf(USER_ERR, "error: undefined key: ");
            ValuePrint(arg, USER_ERR);
            return Error_OutOfRange;
        }

        StackPush(self, result);
        break;
    }

    default:
        fprintf(USER_ERR, "error: cannot get item from %s\n",
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
        fprintf(USER_ERR, "error: wrong number of arguments, expected 1, got %d\n",
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
            fprintf(USER_ERR, "error: index out of range\n");
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
        fprintf(USER_ERR, "error: cannot set item of %s\n",
                ObjectTypeToString(ObjectGetType(obj)));
        return Error_TypeMismatch;
    }

    return Error_None;
}
