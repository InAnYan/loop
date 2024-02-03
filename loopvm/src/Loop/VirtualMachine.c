#include "VirtualMachine.h"

#include "Filesystem.h"
#include "Object.h"
#include "Opcode.h"

#include "Objects/BoundMethod.h"
#include "Objects/Class.h"
#include "Objects/Closure.h"
#include "Objects/String.h"
#include "Objects/Dictionary.h"
#include "Objects/Function.h"
#include "Objects/Instance.h"
#include "Objects/List.h"
#include "Objects/Module.h"
#include "Objects/Upvalue.h"

void CommonObjectsInit(CommonObjects *self, VirtualMachine *vm) {
    self->script = ObjectStringFromLiteral(vm, "script");
    self->init = ObjectStringFromLiteral(vm, "init");
    self->empty_string = ObjectStringFromLiteral(vm, "");
    self->dot_code = ObjectStringFromLiteral(vm, ".code");
    self->compiled_dir = ObjectStringFromLiteral(vm, ".loop_compiled");
}

void CommonObjectsDeinit(CommonObjects *self) {
    self->empty_string = NULL;
    self->init = NULL;
    self->script = NULL;
    self->dot_code = NULL;
    self->compiled_dir = NULL;
}

void CommonObjectsMarkTraverse(CommonObjects *self, MemoryManager *memory) {
    ObjectMark((Object *) self->empty_string, memory);
    ObjectMark((Object *) self->init, memory);
    ObjectMark((Object *) self->script, memory);
    ObjectMark((Object *) self->dot_code, memory);
    ObjectMark((Object *) self->compiled_dir, memory);
}

Error VirtualMachineInit(VirtualMachine *self) {
    MemoryManagerInit(&self->memory_manager, self); // Potential bug, if conf is not set in VM.
    self->memory_manager.on = false;
    self->stack_ptr = self->stack;
    self->frame_ptr = self->frames;
    self->handler_ptr = self->handlers;
    HashTableInit(&self->strings);
    HashTableInitWithCapacity(&self->modules, self);
    CommonObjectsInit(&self->common, self); // Bug if a lot is not set.

    /*
    self->called_path = GetCurrentWorkingDirectory(self);
    if (self->called_path == NULL)
    {
        fprintf(USER_ERR, "FATAL ERROR: failed to get current working directory.\n");
        return Error_IOError;
    }
    */
    // TODO: Remove called path in VM?

    const char *packages_path = getenv("LOOP_PACKAGES_PATH");
    if (packages_path == NULL) {
        fprintf(USER_ERR, "FATAL ERROR: LOOP_PACKAGES_PATH is not set.\n");
        return Error_IOError;
    }

    self->packages_path = ObjectStringFromLiteral(self, packages_path);

    self->open_upvalues = NULL;

    return Error_None;
}

void VirtualMachineDeinit(VirtualMachine *self) {
    assert(self->open_upvalues == NULL);
    self->open_upvalues = NULL;
    self->packages_path = NULL;
    self->called_path = NULL;
    HashTableDeinit(&self->modules, self);
    HashTableDeinit(&self->strings, self);
    self->frame_ptr = NULL;
    self->stack_ptr = NULL;
    CommonObjectsDeinit(&self->common);
    MemoryManagerDeinit(&self->memory_manager);
}

static Error PushScript(VirtualMachine *self, ObjectFunction *script);

static Error PushFrame(VirtualMachine *self, ObjectFunction *function, ObjectClosure *closure);

static Error PopFrame(VirtualMachine *self);

static Value StackPeek(VirtualMachine *self);

static void StackPeekSet(VirtualMachine *self, Value value);

static Value StackPeekAt(VirtualMachine *self, size_t offset);

static Value StackPop(VirtualMachine *self);

static void StackPopSeveral(VirtualMachine *self, size_t count);

static void StackPush(VirtualMachine *self, Value value);

static ObjectModule *GetModule(CallFrame *frame);

static ObjectFunction *GetFunction(CallFrame *frame);

static Value GetGlobal(CallFrame *frame, size_t arg);

static void SetGlobal(CallFrame *frame, size_t arg, Value value);

static HashTable *GetExports(CallFrame *frame);

static uint8_t ReadByte(CallFrame *frame);

static uint16_t ReadShort(CallFrame *frame);

static Value ReadConstant(CallFrame *frame);

static void TraceStack(VirtualMachine *self);

typedef enum BinaryOp {
    BinaryOp_Add,
    BinaryOp_Subtract,
    BinaryOp_Multiply,
    BinaryOp_Divide,
    BinaryOp_Greater,
    BinaryOp_Less,
} BinaryOp;

static Error BinOp(VirtualMachine *self, BinaryOp op);

static Error Call(VirtualMachine *self, Value value, uint8_t arity);

static Error GetItem(VirtualMachine *self, Value value, uint8_t arity);

static Error SetItem(VirtualMachine *self, Value value, uint8_t arity);

static Error GetAttribute(VirtualMachine *self, Value from, Value attr);

static Error SetAttribute(VirtualMachine *self, Value instance, Value key, Value value);

static ObjectUpvalue *CaptureUpvalue(VirtualMachine *self, Value *location);

static void CloseUpvalues(VirtualMachine *self, Value *last);

static Error Run(VirtualMachine *self);

Error VirtualMachineRunScript(VirtualMachine *self, ObjectFunction *script) {
    self->memory_manager.on = true;
    TRY(PushScript(self, script));
    TRY(Run(self));
    self->memory_manager.on = false;
    return Error_None;
}

static ObjectString *MakeCompiledPath(VirtualMachine *self, const ObjectString *path);

static bool InternModule(VirtualMachine *self, ObjectString *path, ObjectModule **ptr);

static Error LoadNewModule(VirtualMachine *self, ObjectString *path, ObjectModule **ptr);

Error VirtualMachineLoadModule(VirtualMachine *self, ObjectString *parent, ObjectString *path, ObjectModule **ptr) {
    ObjectString *parent_paths[] = {parent, self->common.empty_string, self->packages_path};
    ObjectString *constructed_paths[sizeof(parent_paths) / sizeof(parent_paths[0])] = {};

    for (size_t i = 0; i < sizeof(parent_paths) / sizeof(parent_paths[0]); ++i) {
        ObjectString *parent_path = parent_paths[i];
        ObjectString *compiled_path = MakeCompiledPath(self, path);
        ObjectString *combined_path = JoinPath(self, parent_path, compiled_path);
        ObjectString *abs_path = GetAbsolutePath(self, combined_path);

        constructed_paths[i] = abs_path;

        if (abs_path == NULL) {
            continue;
        }

        if (InternModule(self, abs_path, ptr)) {
            return Error_None;
        }
    }

    for (size_t i = 0; i < sizeof(constructed_paths) / sizeof(constructed_paths[0]); ++i) {
        ObjectString *the_path = constructed_paths[i];

        if (the_path == NULL) {
            continue;
        }

        if (DoesPathExists(the_path)) {
            return LoadNewModule(self, the_path, ptr);
        }
    }

    // TODO: This error print shows .code extension.
    fprintf(USER_ERR, "error: module '%s' not found.\n", path->str);
    return Error_FileNotFound;
}

static ObjectString *MakeCompiledPath(VirtualMachine *self, const ObjectString *path) {
    ObjectString *dir = GetDirName(self, path);
    ObjectString *base = GetBaseName(self, path);
    ObjectString *changed_dir = JoinPath(self, dir, self->common.compiled_dir, base);
    return ObjectStringConcatenate(self, changed_dir, self->common.dot_code);
}

static bool InternModule(VirtualMachine *self, ObjectString *path, ObjectModule **ptr) {
    Value interned;
    if (!HashTableGet(&self->modules, ValueObject((Object *) path), &interned)) {
        return false;
    }

    assert(ValueIsObject(interned) && ObjectIsModule(ValueAsObject(interned)));
    *ptr = ObjectAsModule(ValueAsObject(interned));
    return true;
}

static Error LoadNewModule(VirtualMachine *self, ObjectString *path, ObjectModule **ptr) {
    char *buffer = NULL;
    TRY(ReadFileWithComments(USER_ERR, path->str, &buffer));

    cJSON *data = cJSON_Parse(buffer);
    if (data == NULL) {
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

#define CHECK_VALUE_OBJECT_TYPE(self, value, type) \
    do \
    { \
        if (!ValueIsObject(value)) \
        { \
            fprintf(USER_ERR, "error: expected %s, got %s\n", \
                #type, ValueTypeToString(ValueGetType(value))); \
            return Error_TypeMismatch; \
        } \
        if (!ObjectIs##type(ValueAsObject(value))) \
        { \
            fprintf(USER_ERR, "error: expected %s, got %s\n", \
                #type, ObjectTypeToString(ObjectGetType(ValueAsObject(value)))); \
            return Error_TypeMismatch; \
        } \
    } while (0)

Error Run(VirtualMachine *self) {
    while (true) {
        CallFrame *frame = &self->frame_ptr[-1];

#ifdef VM_TRACE_EXECUTION

        TraceStack(self);
        ChunkDisassembleInstruction(&frame->function->chunk, DEBUG_OUT, frame->ip);

#endif

        Opcode opcode = ReadByte(frame);

        switch (opcode) {
            case Opcode_PushConstant: {
                Value value = ReadConstant(frame);
                StackPush(self, value);
                break;
            }

            case Opcode_PushFalse: {
                StackPush(self, ValueBool(false));
                break;
            }

            case Opcode_PushTrue: {
                StackPush(self, ValueBool(true));
                break;
            }

            case Opcode_PushNull: {
                StackPush(self, ValueNull());
                break;
            }

            case Opcode_Negate: {
                Value value = StackPeek(self);
                CHECK_VALUE_TYPE(self, value, Int);
                StackPeekSet(self, ValueInt(-ValueAsInt(value)));
                break;
            }

            case Opcode_Not: {
                Value value = StackPeek(self);
                StackPeekSet(self, ValueBool(ValueIsFalse(value)));
                break;
            }

            case Opcode_Plus: {
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

            case Opcode_Equal: {
                // TODO: Objects custom equality.
                Value b = StackPop(self);
                Value a = StackPeek(self);
                StackPeekSet(self, ValueBool(ValueAreEqual(a, b)));
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

            case Opcode_Print: {
                // TODO: Objects custom printing.
                Value value = StackPeek(self);
                ValuePrint(value, USER_OUT);
                fprintf(USER_OUT, "\n");
                StackPop(self);
                break;
            }

            case Opcode_Pop: {
                StackPop(self);
                break;
            }

            case Opcode_GetGlobal: {
                uint8_t arg = ReadByte(frame);
                Value value = GetGlobal(frame, arg);
                StackPush(self, value);
                break;
            }

            case Opcode_SetGlobal: {
                uint8_t arg = ReadByte(frame);
                Value value = StackPeek(self);
                SetGlobal(frame, arg, value);
                break;
            }

            case Opcode_GetLocal: {
                uint8_t slot = ReadByte(frame);
                StackPush(self, frame->locals[slot]);
                break;
            }

            case Opcode_SetLocal: {
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

            case Opcode_Return: {
                // TODO: uhm, probably bug with the first script that is very last at the end.

                Value value = StackPop(self);

                TRY(PopFrame(self));

                if (self->frame_ptr == self->frames) {
                    return Error_None;
                }

                StackPush(self, value);

                break;
            }

            case Opcode_Export: {
                Value value = StackPeek(self);
                Value key = ReadConstant(frame);

                if (!HashTablePut(GetExports(frame), self, key, value)) {
                    fprintf(USER_ERR,
                            "error: variable reexport: '%s'\n",
                            ObjectAsString(ValueAsObject(key))->str);
                    return Error_VariableRedefinition;
                }

                StackPop(self);

                break;
            }

            case Opcode_Import: {
                // TODO: BUG in compiler first statement is import wrong line number.
                // TODO: Or maybe there.

                Value key = ReadConstant(frame);
                ObjectString *str = ObjectAsString(ValueAsObject(key));

                ObjectModule *module = NULL;

                self->memory_manager.on = false;
                TRY(VirtualMachineLoadModule(self, frame->function->module->parent_dir, str, &module));
                self->memory_manager.on = true;

                if (module->state == ObjectModuleState_ScriptNotExecuted) {
                    module->state = ObjectModuleState_ScriptRunning;
                    TRY(PushScript(self, module->script));
                } else if (module->state == ObjectModuleState_ScriptRunning) {
                    // TODO: Better error message.
                    fprintf(USER_ERR, "error: circular import: '%s'\n", str->str);
                    return Error_CircularImport;
                }

                break;
            }

            case Opcode_Top: // TODO: Delete? I don't use it, probably.
            {
                StackPush(self, StackPeek(self));
                break;
            }

            case Opcode_GetAttribute: {
                Value attr = ReadConstant(frame);
                Value from = StackPeek(self);

                TRY(GetAttribute(self, from, attr));

                break;
            }

            case Opcode_SetAttribute: {
                Value key = ReadConstant(frame);
                Value value = StackPeek(self);
                Value instance = StackPeekAt(self, 1);

                TRY(SetAttribute(self, instance, key, value));

                break;
            }

            case Opcode_ModuleEnd: {
                // Code duplication(
                // Please forgive me.

                StackPop(self);

                frame->function->module->state = ObjectModuleState_ScriptExecuted;
                Value module = ValueObject((Object *) frame->function->module);

                TRY(PopFrame(self));

                if (self->frame_ptr == self->frames) {
                    return Error_None;
                }

                StackPush(self, module);

                break;
            }

            case Opcode_BuildDictionary: {
                uint8_t count = ReadByte(frame);
                assert(count % 2 == 0);

                ObjectDictionary *obj = ObjectDictionaryNew(self);
                StackPush(self, ValueObject((Object *) obj)); // Interesting bug.

                for (int i = 0; i < count; ++i) {
                    Value value = StackPeekAt(self, i * 2 + 1);
                    Value key = StackPeekAt(self, i * 2 + 2);
                    HashTablePut(&obj->entries, self, key, value);
                }

                StackPopSeveral(self, count * 2 + 1);

                StackPush(self, ValueObject((Object *) obj));

                break;
            }

            case Opcode_GetExport: {
                Value key = ReadConstant(frame);
                Value value;
                if (!HashTableGet(GetExports(frame), key, &value)) {
                    fprintf(USER_ERR, "error: variable not exported: '%s'\n", ObjectAsString(ValueAsObject(key))->str);
                    return Error_UndefinedReference;
                }
                break;
            }

            case Opcode_SetExport: {
                Value key = ReadConstant(frame);
                Value value = StackPeek(self);
                if (!HashTablePut(GetExports(frame), self, key, value)) {
                    fprintf(USER_ERR, "error: variable not exported: '%s'\n", ObjectAsString(ValueAsObject(key))->str);
                    return Error_UndefinedReference;
                }
                break;
            }

            case Opcode_GetUpvalue: {
                uint8_t index = ReadByte(frame);
                assert(frame->closure);
                assert(index < frame->closure->upvalue_count);
                StackPush(self, *frame->closure->upvalues[index]->location);
                break;
            }

            case Opcode_SetUpvalue: {
                uint8_t index = ReadByte(frame);
                assert(frame->closure);
                assert(index < frame->closure->upvalue_count);
                *frame->closure->upvalues[index]->location = StackPeek(self);
                break;
            }

            case Opcode_BuildClosure: {
                ObjectFunction *func = ObjectAsFunction(ValueAsObject(StackPeek(self)));
                int count = ReadByte(frame);
                ObjectClosure *closure = ObjectClosureNew(self, func, count);
                StackPop(self); // func is inside the closure.
                StackPush(self, ValueObject((Object *) closure));

                for (int i = 0; i < count; ++i) {
                    bool is_local = ReadByte(frame);
                    uint8_t index = ReadByte(frame);

                    if (is_local) {
                        closure->upvalues[i] = CaptureUpvalue(self, frame->locals + index);
                    } else {
                        assert(frame->closure);
                        closure->upvalues[i] = frame->closure->upvalues[index];
                    }
                }

                break;
            }

            case Opcode_CloseUpvalue: {
                CloseUpvalues(self, self->stack_ptr - 1);
                StackPop(self);
                break;
            }

            case Opcode_BuildList: {
                uint8_t count = ReadByte(frame);

                ObjectList *obj = ObjectListNew(self);
                StackPush(self, ValueObject((Object *) obj));

                for (int i = count - 1; i >= 0; --i) {
                    ObjectListPush(obj, self, StackPeekAt(self, i + 1));
                }

                StackPopSeveral(self, count + 1);

                StackPush(self, ValueObject((Object *) obj));

                break;
            }

            case Opcode_Inherit: {
                Value parent = StackPeek(self);
                Value child = StackPeekAt(self, 1);

                CHECK_VALUE_OBJECT_TYPE(self, parent, Class);
                CHECK_VALUE_OBJECT_TYPE(self, child, Class);

                ObjectClass *parent_class = ObjectAsClass(ValueAsObject(parent));
                ObjectClass *child_class = ObjectAsClass(ValueAsObject(child));

                child_class->super = parent_class;
                HashTableAddAll(&child_class->methods, self, &parent_class->methods);

                StackPop(self);

                break;
            }

            case Opcode_SuperGet: {
                Value name = ReadConstant(frame);
                Value instance = frame->locals[0];

                CHECK_VALUE_OBJECT_TYPE(self, instance, Instance);

                ObjectInstance *instance_obj = ObjectAsInstance(ValueAsObject(instance));
                ObjectClass *klass = instance_obj->klass;

                if (klass->super == NULL) {
                    fprintf(USER_ERR, "error: no super class\n");
                    return Error_UndefinedReference;
                }

                Value method;
                if (!HashTableGet(&klass->super->methods, name, &method)) {
                    fprintf(USER_ERR, "error: undefined property '%s'\n", ObjectAsString(ValueAsObject(name))->str);
                    return Error_UndefinedReference;
                }

                // TODO: Bound method fields. Value maybe?
                // TODO: ObjectAsFunction should apply (or not) a closure. Oh, actually methods can't be closures.

                ObjectBoundMethod *bound_method = ObjectBoundMethodNew(self, instance_obj,
                                                                       ObjectAsFunction(ValueAsObject(method)));

                StackPush(self, ValueObject((Object *) bound_method));

                break;
            }

            case Opcode_TryBegin:
            {
                uint16_t jump = ReadShort(frame);

                if (self->handler_ptr - self->handlers >= VM_HANDLERS_COUNT) {
                    fprintf(USER_ERR, "error: too many catch handlers\n");
                    return Error_StackOverflow;
                }

                CatchHandler *handler = self->handler_ptr++;
                handler->frame = frame;
                handler->ip = frame->ip + jump;
                handler->stack_ptr = self->stack_ptr;
                handler->open_upvalues = self->open_upvalues;

                break;
            }

            case Opcode_TryEnd:
            {
                assert(self->handler_ptr != self->handlers);
                --self->handler_ptr;
                break;
            }

            case Opcode_Throw:
            {
                Value value = StackPop(self);

                if (self->handler_ptr == self->handlers) {
                    fprintf(USER_ERR, "error: unhandled exception\n");
                    return Error_UnhandledException;
                }

                CatchHandler* handler = --self->handler_ptr;
                frame = handler->frame;
                frame->ip = handler->ip;
                self->stack_ptr = handler->stack_ptr;
                self->open_upvalues = handler->open_upvalues;
                StackPush(self, value);

                break;
            }

            default:
                fprintf(USER_ERR, "FATAL ERROR: unknown opcode: 0x%02x\n", opcode);
                return Error_UnknownOpcode;
        }
    }
}

static ObjectUpvalue *CaptureUpvalue(VirtualMachine *self, Value *location) {
    // TODO: Sort linked list.

    ObjectUpvalue *current = self->open_upvalues;
    while (current && current->location != location) {
        current = current->next;
    }

    if (current) {
        return current;
    }

    ObjectUpvalue *upvalue = ObjectUpvalueNew(self, location, self->open_upvalues);
    self->open_upvalues = upvalue;
    return upvalue;
}

static void CloseUpvalues(VirtualMachine *self, Value *last) {
    ObjectUpvalue *prev = NULL;
    ObjectUpvalue *cur = self->open_upvalues;
    while (cur) {
        if (cur->location >= last) {
            cur->closed = *cur->location;
            cur->location = &cur->closed;

            if (prev) {
                prev->next = cur->next;
            } else {
                self->open_upvalues = cur->next;
            }
        } else {
            prev = cur;
        }

        cur = cur->next;
    }
}

static Error PushScript(VirtualMachine *self, ObjectFunction *script) {
    StackPush(self, ValueObject((Object *) script));

    return PushFrame(self, script, NULL);
}

static Error PushFrame(VirtualMachine *self, ObjectFunction *function, ObjectClosure *closure) {
    if (self->frame_ptr - self->frames == VM_FRAMES_COUNT) {
        // TODO: ERROR?
        return Error_StackOverflow;
    }

    CallFrame *frame = self->frame_ptr++;
    frame->function = function;
    frame->closure = closure;
    frame->ip = function->chunk.code;
    frame->locals = self->stack_ptr - function->arity - 1;

    return Error_None;
}

static Error PopFrame(VirtualMachine *self) {
    if (self->frame_ptr == self->frames) {
        // TODO: ERROR?
        return Error_StackUnderflow;
    }

    // To close captured arguments.
    CloseUpvalues(self, self->frame_ptr[-1].locals);

    --self->frame_ptr;
    self->stack_ptr = self->frame_ptr->locals;

    return Error_None;
}

// TODO: Asserts in stack operations.

static Value StackPeek(VirtualMachine *self) {
    return self->stack_ptr[-1];
}

static void StackPeekSet(VirtualMachine *self, Value value) {
    self->stack_ptr[-1] = value;
}

static Value StackPeekAt(VirtualMachine *self, size_t offset) {
    return self->stack_ptr[-1 - offset];
}

static Value StackPop(VirtualMachine *self) {
    return *--self->stack_ptr;
}

static void StackPopSeveral(VirtualMachine *self, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        StackPop(self);
    }
}

static void StackPush(VirtualMachine *self, Value value) {
    *self->stack_ptr++ = value;
}

static ObjectModule *GetModule(CallFrame *frame) {
    return frame->function->module;
}

static ObjectFunction *GetFunction(CallFrame *frame) {
    return frame->function;
}

static Value GetGlobal(CallFrame *frame, size_t arg) {
    assert(arg < GetModule(frame)->globals_count);
    return GetModule(frame)->globals[arg];
}

static void SetGlobal(CallFrame *frame, size_t arg, Value value) {
    assert(arg < GetModule(frame)->globals_count);
    GetModule(frame)->globals[arg] = value;
}

static HashTable *GetExports(CallFrame *frame) {
    return &frame->function->module->exports;
}

static uint8_t ReadByte(CallFrame *frame) {
    assert(frame->ip != frame->function->chunk.code + frame->function->chunk.code_length);
    return *(frame->ip++);
}

static uint16_t ReadShort(CallFrame *frame) {
    uint16_t part_1 = ReadByte(frame);
    uint16_t part_2 = ReadByte(frame);
    return (part_2 << 8) | part_1;
}

static Value ReadConstant(CallFrame *frame) {
    uint8_t index = ReadByte(frame);
    assert(index < frame->function->chunk.constants_length);
    return frame->function->chunk.constants[index];
}

static void TraceStack(VirtualMachine *self) {
    FILE *out = DEBUG_OUT;

    for (Value *ptr = self->stack; ptr != self->stack_ptr; ++ptr) {
        fprintf(out, "[ ");
        ValuePrint(*ptr, out);
        fprintf(out, " ]");
    }

    fprintf(out, "\n");
}

static Error BinOp(VirtualMachine *self, BinaryOp op) {
    // TODO: Operator overload.
    Value b = StackPop(self);
    Value a = StackPop(self);

    CHECK_VALUE_TYPE(self, a, Int);
    CHECK_VALUE_TYPE(self, b, Int);

    int rhs = ValueAsInt(b);
    int lhs = ValueAsInt(a);

    switch (op) {
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
            if (rhs == 0) {
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

static Error Call(VirtualMachine *self, Value value, uint8_t arity) {
    if (!ValueIsObject(value)) {
        fprintf(USER_ERR, "error: expected callable, got %s\n",
                ValueTypeToString(ValueGetType(value)));
        return Error_NonCallable;
    }

    Object *obj = ValueAsObject(value);
    ObjectType obj_type = ObjectGetType(obj);

    switch (obj_type) {
        case ObjectType_Function: {
            ObjectFunction *func = ObjectAsFunction(obj);

            if (func->arity != arity) {
                fprintf(USER_ERR, "error: wrong number of arguments, expected %zu, got %d\n",
                        func->arity, arity);
                return Error_WrongArgumentsCount;
            }

            TRY(PushFrame(self, func, NULL));

            break;
        }

        case ObjectType_Closure: {
            // Oh, no. Code duplication. Again.

            ObjectClosure *closure = ObjectAsClosure(obj);

            if (closure->function->arity != arity) {
                fprintf(USER_ERR, "error: wrong number of arguments, expected %zu, got %d\n",
                        closure->function->arity, arity);
                return Error_WrongArgumentsCount;
            }

            TRY(PushFrame(self, closure->function, closure));

            break;
        }

        case ObjectType_Class: {
            ObjectClass *klass = ObjectAsClass(obj);
            ObjectInstance *instance = ObjectInstanceNew(self, klass);

            self->stack_ptr[-arity - 1] = ValueObject((Object *) instance);

            Value init;
            if (HashTableGet(&klass->methods, ValueObject((Object *) self->common.init), &init)) {
                return Call(self, init, arity);
            }

            if (arity != 0) {
                fprintf(USER_ERR, "error: wrong number of arguments, expected 0, got %d\n",
                        arity);
                return Error_WrongArgumentsCount;
            }

            break;
        }

        case ObjectType_BoundMethod: {
            ObjectBoundMethod *bound = ObjectAsBoundMethod(obj);
            self->stack_ptr[-arity - 1] = ValueObject((Object *) bound->receiver);
            return Call(self, ValueObject((Object *) bound->method), arity);
        }

        default:
            fprintf(USER_ERR, "error: expected callable, got %s\n",
                    ObjectTypeToString(obj_type));
            return Error_NonCallable;
    }

    return Error_None;
}

static Error GetObjectAttribute(VirtualMachine *self, Object *obj, Value key);

static Error GetAttribute(VirtualMachine *self, Value from, Value attr) {
    switch (ValueGetType(from)) {
        case ValueType_Object:
            return GetObjectAttribute(self, ValueAsObject(from), attr);
        default:
            fprintf(USER_ERR, "error: cannot get attribute from %s\n", ValueTypeToString(ValueGetType(from)));
            return Error_TypeMismatch;
    }
}

static Error GetObjectAttribute(VirtualMachine *self, Object *obj, Value key) {
    switch (ObjectGetType(obj)) {
        case ObjectType_Module: {
            ObjectModule *module = ObjectAsModule(obj);

            Value value;
            if (HashTableGet(&module->exports, key, &value)) {
                StackPop(self);
                StackPush(self, value);
                return Error_None;
            }

            fprintf(USER_ERR, "error: undefined export: '%s'\n", ObjectAsString(ValueAsObject(key))->str);
            return Error_UndefinedReference;
        }

        case ObjectType_Instance: {
            ObjectInstance *instance = ObjectAsInstance(obj);
            Value value;

            if (HashTableGet(&instance->fields, key, &value)) {
                StackPop(self);
                StackPush(self, value);
                return Error_None;
            }

            if (HashTableGet(&instance->klass->methods, key, &value)) {
                assert(ObjectIsFunction(ValueAsObject(value)));
                ObjectBoundMethod *bound = ObjectBoundMethodNew(self,
                                                                instance,
                                                                ObjectAsFunction(ValueAsObject(value)));
                StackPop(self);
                StackPush(self, ValueObject((Object *) bound));
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

static Error SetObjectAttribute(VirtualMachine *self, Object *instance, Value key, Value value);

static Error SetAttribute(VirtualMachine *self, Value instance, Value key, Value value) {
    switch (ValueGetType(instance)) {
        case ValueType_Object:
            return SetObjectAttribute(self, ValueAsObject(instance), key, value);
        default:
            fprintf(USER_ERR, "error: cannot set attribute for %s\n", ValueTypeToString(ValueGetType(instance)));
            return Error_TypeMismatch;
    }
}

static Error SetObjectAttribute(VirtualMachine *self, Object *instance, Value key, Value value) {
    if (!ObjectIsInstance(instance)) {
        fprintf(USER_ERR, "error: expected Instance, got %s\n",
                ObjectTypeToString(ObjectGetType(instance)));
        return Error_TypeMismatch;
    }

    ObjectInstance *obj = ObjectAsInstance(instance);

    HashTablePut(&obj->fields, self, key, value);
    StackPop(self); // value
    StackPop(self); // instance
    StackPush(self, value);

    return Error_None;
}

static Error GetItem(VirtualMachine *self, Value value, uint8_t arity) {
    CHECK_VALUE_TYPE(self, value, Object);

    // TODO: This arity check is not general.
    if (arity != 1) {
        fprintf(USER_ERR, "error: wrong number of arguments, expected 1, got %d\n",
                arity);
        return Error_WrongArgumentsCount;
    }

    Value arg = StackPeek(self);

    Object *obj = ValueAsObject(value);

    Value res;

    switch (ObjectGetType(obj)) {
        case ObjectType_String: {
            ObjectString *str = ObjectAsString(obj);

            CHECK_VALUE_TYPE(self, arg, Int);

            int index = ValueAsInt(arg);
            if (index < 0 || index >= str->length) {
                fprintf(USER_ERR, "error: index out of range\n");
                return Error_OutOfRange;
            }

            res = ValueObject((Object *) ObjectStringSubstring(self, str, index, index + 1));

            break;
        }

        case ObjectType_Dictionary: {
            ObjectDictionary *dictionary = ObjectAsDictionary(obj);

            if (!HashTableGet(&dictionary->entries, arg, &res)) {
                fprintf(USER_ERR, "error: undefined key: ");
                ValuePrint(arg, USER_ERR);
                return Error_OutOfRange;
            }

            break;
        }

        case ObjectType_List: {
            ObjectList *list = ObjectAsList(obj);

            CHECK_VALUE_TYPE(self, arg, Int);

            int index = ValueAsInt(arg);
            if (index < 0 || index >= list->count) {
                fprintf(USER_ERR, "error: index out of range\n");
                return Error_OutOfRange;
            }

            res = list->elements[index];

            break;
        }

        default:
            fprintf(USER_ERR, "error: cannot get item from %s\n",
                    ObjectTypeToString(ObjectGetType(obj)));
            return Error_TypeMismatch;
    }

    StackPop(self);
    StackPop(self);

    StackPush(self, res);

    return Error_None;
}

static Error SetItem(VirtualMachine *self, Value value, uint8_t arity) {
    CHECK_VALUE_TYPE(self, value, Object);

    // TODO: This arity check is not general.
    if (arity != 2) {
        fprintf(USER_ERR, "error: wrong number of arguments, expected 1, got %d\n",
                arity);
        return Error_WrongArgumentsCount;
    }

    Value assign = StackPeek(self);
    Value arg = StackPeekAt(self, 1);

    Object *obj = ValueAsObject(value);

    switch (ObjectGetType(obj)) {
        case ObjectType_Dictionary: {
            ObjectDictionary *dictionary = ObjectAsDictionary(obj);

            HashTablePut(&dictionary->entries, self, arg, assign);
            break;
        }

        case ObjectType_List: {
            ObjectList *list = ObjectAsList(obj);

            CHECK_VALUE_TYPE(self, arg, Int);

            int index = ValueAsInt(arg);
            if (index < 0 || index >= list->count) {
                fprintf(USER_ERR, "error: index out of range\n");
                return Error_OutOfRange;
            }

            list->elements[index] = assign;

            break;
        }

        default:
            fprintf(USER_ERR, "error: cannot set item of %s\n",
                    ObjectTypeToString(ObjectGetType(obj)));
            return Error_TypeMismatch;
    }

    StackPop(self); // value
    StackPop(self); // arg
    StackPop(self); // instance

    StackPush(self, assign);

    return Error_None;
}

void VirtualMachineMarkRoots(VirtualMachine *self, MemoryManager *memory) {
    CommonObjectsMarkTraverse(&self->common, memory);

    for (Value *slot = self->stack; slot != self->stack_ptr; ++slot) {
        ValueMark(*slot, memory);
    }

    // ObjectMark((Object*)self->called_path, memory);
    ObjectMark((Object *) self->packages_path, memory);
}
