#ifndef LOOP_CHUNK_H
#define LOOP_CHUNK_H

#include "Common.h"

typedef struct Chunk
{
    uint8_t* code;
    size_t code_length;
    size_t code_capacity;
    Value* constants;
    size_t constants_length;
    size_t constants_capacity;
    size_t* lines;
    size_t lines_length;
    size_t lines_capacity;
} Chunk;

void ChunkInit(Chunk* self);
void ChunkDeinit(Chunk* self, VirtualMachine* vm);
void ChunkFromJSON(Chunk* self, VirtualMachine* vm, ObjectModule* module, const cJSON* json);
size_t ChunkGetLine(const Chunk* self, size_t offset);
void ChunkDisassemble(const Chunk* self, FILE* out, const char* name);
const uint8_t* ChunkDisassembleInstruction(const Chunk* self, FILE* out, const uint8_t* offset);

void ChunkMarkTraverse(Chunk* self, MemoryManager* memory);

#endif // LOOP_CHUNK_H
