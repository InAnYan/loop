#include "Chunk.h"

#include "MemoryManager.h"
#include "Opcode.h"
#include "VirtualMachine.h"

static void PushCode(Chunk* self, VirtualMachine* vm, uint8_t byte);
static void PushConstant(Chunk* self, VirtualMachine* vm, Value value);
static void PushLine(Chunk* self, VirtualMachine* vm, size_t line);

void ChunkInit(Chunk* self)
{
    self->code = NULL;
    self->code_length = 0;
    self->code_capacity = 0;
    self->constants = NULL;
    self->constants_length = 0;
    self->constants_capacity = 0;
    self->lines = NULL;
    self->lines_length = 0;
    self->lines_capacity = 0;
}

void ChunkDeinit(Chunk* self, VirtualMachine* vm)
{
    FREE_ARRAY(vm, self->code, uint8_t, self->code_capacity);
    FREE_ARRAY(vm, self->constants, Value, self->constants_capacity);
    FREE_ARRAY(vm, self->lines, size_t, self->lines_capacity);
    ChunkInit(self);
}

void ChunkFromJSON(Chunk* self, VirtualMachine* vm, ObjectModule* module, const cJSON* json)
{
    assert(cJSON_IsObject(json));

    const cJSON* code = cJSON_GetObjectItemCaseSensitive(json, "code");
    assert(cJSON_IsArray(code));

    const cJSON* byte = NULL;
    cJSON_ArrayForEach(byte, code)
    {
        assert(cJSON_IsNumber(byte));
        PushCode(self, vm, byte->valueint);
    }

    const cJSON* constants = cJSON_GetObjectItemCaseSensitive(json, "constants");
    assert(cJSON_IsArray(constants));

    const cJSON* constant = NULL;
    cJSON_ArrayForEach(constant, constants)
    {
        assert(cJSON_IsObject(constant));
        PushConstant(self, vm, ValueFromJSON(vm, module, constant));
    }

    const cJSON* lines = cJSON_GetObjectItemCaseSensitive(json, "lines");
    assert(cJSON_IsArray(lines));

    const cJSON* line = NULL;
    cJSON_ArrayForEach(line, lines)
    {
        assert(cJSON_IsNumber(line));
        PushLine(self, vm, line->valueint);
    }
}

// Oh, no. Code duplication.

static void PushCode(Chunk* self, VirtualMachine* vm, uint8_t byte)
{
    if (self->code_length + 1 > self->code_capacity)
    {
        const size_t new_capacity = GROW_CAPACITY(self->code_capacity);
        self->code = REALLOC_ARRAY(vm, self->code, uint8_t, new_capacity, self->code_capacity);
        self->code_capacity = new_capacity;
    }

    self->code[self->code_length++] = byte;
}

static void PushConstant(Chunk* self, VirtualMachine* vm, Value value)
{
    if (self->constants_length + 1 > self->constants_capacity)
    {
        const size_t new_capacity = GROW_CAPACITY(self->constants_capacity);
        self->constants = REALLOC_ARRAY(vm, self->constants, Value, new_capacity, self->constants_capacity);
        self->constants_capacity = new_capacity;
    }

    self->constants[self->constants_length++] = value;
}

static void PushLine(Chunk* self, VirtualMachine* vm, size_t byte)
{
    if (self->lines_length + 1 > self->lines_capacity)
    {
        const size_t new_capacity = GROW_CAPACITY(self->lines_capacity);
        self->lines = REALLOC_ARRAY(vm, self->lines, size_t, new_capacity, self->lines_capacity);
        self->lines_capacity = new_capacity;
    }

    self->lines[self->lines_length++] = byte;
}

size_t ChunkGetLine(const Chunk* self, size_t offset)
{
    size_t counter = 0;

    for (int i = 0; i < self->lines_length; ++i)
    {
        for (int j = self->lines[i]; j >= 0; --j)
        {
            ++counter;

            if (counter == offset)
            {
                return i;
            }
        }
    }

    assert(self->lines_length != 0);
    return self->lines_length - 1;
}

void ChunkDisassemble(const Chunk* self, FILE* out, const char* name)
{
    fprintf(out, "=== %s ===\n", name);

    for (const uint8_t* ptr = self->code;
         ptr != self->code + self->code_length;
         ptr = ChunkDisassembleInstruction(self, out, ptr));
}

const uint8_t* DisassembleSimple(const Chunk* self, FILE* out, const uint8_t* offset, const char* name);
const uint8_t* DisassembleConstant(const Chunk* self, FILE* out, const uint8_t* offset, const char* name);
const uint8_t* DisassembleJump(const Chunk* self, FILE* out, const uint8_t* offset, const char* name);
const uint8_t* DisassembleByte(const Chunk* self, FILE* out, const uint8_t* offset, const char* name);
const uint8_t* DisassembleLoop(const Chunk* self, FILE* out, const uint8_t* offset, const char* name);
const uint8_t* DisassembleUnknown(const Chunk* self, FILE* out, const uint8_t* offset, uint8_t byte);

const uint8_t* ChunkDisassembleInstruction(const Chunk* self, FILE* out, const uint8_t* offset)
{
    const size_t offset_num = offset - self->code;

    fprintf(out, "%04ld ", offset_num);

    if (offset_num != 0 && ChunkGetLine(self, offset_num - 1) == ChunkGetLine(self, offset_num)) // Bad algorithm.
    {
        fprintf(out, "   | ");
    }
    else
    {
        fprintf(out, "%4ld ", ChunkGetLine(self, offset_num));
    }

    const uint8_t byte = offset[0];
    const Opcode opcode = byte;
    switch (opcode)
    {
        #define OPCODE_DISASM(name, type) \
            case Opcode_##name: \
                return Disassemble##type(self, out, offset, #name);

    Opcode_LIST(OPCODE_DISASM)

        #undef OPCODE_DISASM

    default:
        return DisassembleUnknown(self, out, offset, byte);
    }
}

const uint8_t* DisassembleSimple(const Chunk* self, FILE* out, const uint8_t* offset, const char* name)
{
    fprintf(out, "%s\n", name);
    return offset + 1;
}

const uint8_t* DisassembleConstant(const Chunk* self, FILE* out, const uint8_t* offset, const char* name)
{
    uint8_t index = offset[1];
    Value value = self->constants[index];

    fprintf(out, "%-16s %4d ", name, index);
    ValuePrint(value, out);
    fprintf(out, "\n");

    return offset + 2;
}

const uint8_t* DisassembleJump(const Chunk* self, FILE* out, const uint8_t* offset, const char* name)
{
    // TODO: Endianness.
    unsigned part_1 = offset[1];
    unsigned part_2 = offset[2];

    unsigned jump = part_2 << 8 | part_1;

    unsigned arg = (offset - self->code) + jump + 3;

    fprintf(out, "%-16s %04d\n", name, arg);
    return offset + 3;
}

const uint8_t* DisassembleByte(const Chunk* self, FILE* out, const uint8_t* offset, const char* name)
{
    fprintf(out, "%-16s %4d\n", name, offset[1]);
    return offset + 2;
}

const uint8_t* DisassembleLoop(const Chunk* self, FILE* out, const uint8_t* offset, const char* name)
{
    // TODO: Endianness.
    unsigned part_1 = offset[1];
    unsigned part_2 = offset[2];

    unsigned jump = part_2 << 8 | part_1;

    unsigned arg = (offset - self->code) - jump + 3;

    fprintf(out, "%-16s %04d\n", name, arg);
    return offset + 3;
}

const uint8_t* DisassembleUnknown(const Chunk* self, FILE* out, const uint8_t* offset, uint8_t byte)
{
    fprintf(out, "Unknown: 0x%02x\n", byte);
    return offset + 1;
}
