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
    o(DefineGlobal, Constant) \
    o(GetGlobal, Constant) \
    o(SetGlobal, Constant) \
    o(GetLocal, Byte) \
    o(SetLocal, Byte) \
    o(JumpIfFalsePop, Jump) \
    o(Jump, Jump) \
    o(Loop, Loop) \
    o(Call, Byte) \
    o(Export, Simple) \
    o(Import, Constant) \
    o(Top, Simple) \
    o(GetAttribute, Constant) \
    o(ModuleEnd, Simple) \
    o(BuildDictionary, Byte) \
    o(GetItem, Byte) \
    o(SetItem, Byte)

typedef enum Opcode
{
    #define Opcode_ENUM(name, _) Opcode_##name,

    Opcode_LIST(Opcode_ENUM)

    #undef Opcode_ENUM
} Opcode;

const char* OpcodeToString(Opcode value);

#endif // LOOP_OPCODE_H
