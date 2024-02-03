#ifndef LOOP_OPCODE_H
#define LOOP_OPCODE_H

#define Opcode_LIST(o) \
    o(Return, Simple) \
    o(PushConstant, Constant) \
    o(Negate, Simple) \
    o(Add, Simple) \
    o(Subtract, Simple) \
    o(Multiply, Simple) \
    o(Divide, Simple) \
    o(Print, Simple) \
    o(Pop, Simple) \
    o(Plus, Simple) \
    o(Equal, Simple) \
    o(Not, Simple) \
    o(JumpIfFalse, Jump) \
    o(JumpIfTrue, Jump) \
    o(PushTrue, Simple) \
    o(PushFalse, Simple) \
    o(Greater, Simple) \
    o(Less, Simple) \
    o(PushNull, Simple) \
    o(BuildList, Byte) \
    o(GetGlobal, Byte) \
    o(SetGlobal, Byte) \
    o(GetLocal, Byte) \
    o(SetLocal, Byte) \
    o(JumpIfFalsePop, Jump) \
    o(Jump, Jump) \
    o(Loop, Loop) \
    o(Call, Byte) \
    o(Export, Constant) \
    o(Import, Constant) \
    o(Top, Simple) \
    o(GetAttribute, Constant) \
    o(ModuleEnd, Simple) \
    o(BuildDictionary, Byte) \
    o(GetItem, Byte) \
    o(SetItem, Byte) \
    o(SetAttribute, Constant) \
    o(GetExport, Constant) \
    o(SetExport, Constant) \
    o(BuildClosure, Closure) \
    o(GetUpvalue, Byte) \
    o(SetUpvalue, Byte) \
    o(CloseUpvalue, Simple) \
    o(Inherit, Simple) \
    o(SuperGet, Constant)     \
    o(TryBegin, Jump)  \
    o(TryEnd, Simple)  \
    o(Throw, Simple)

typedef enum Opcode {
#define Opcode_ENUM(name, _) Opcode_##name,

    Opcode_LIST(Opcode_ENUM)

#undef Opcode_ENUM
} Opcode;

const char *OpcodeToString(Opcode value);

#endif // LOOP_OPCODE_H
