#include "Module.h"

#include "../VirtualMachine.h"

#include "Function.h"
#include "String.h"
#include "../Filesystem.h"

ObjectModule *ObjectModuleNew(VirtualMachine *vm, ObjectString *name, ObjectString *parent_dir, size_t globals_count) {
    assert(name != NULL);
    assert(parent_dir != NULL);

    ObjectModule *obj = ALLOCATE_OBJECT(vm, Module);

    ObjectFunction *script = ObjectFunctionNew(vm, obj, vm->common.script, 0);

    obj->name = name;
    obj->parent_dir = parent_dir;
    obj->script = script;
    HashTableInit(&obj->exports);
    obj->globals_count = globals_count;
    obj->globals = ALLOC_ARRAY(vm, Value, globals_count);
    for (size_t i = 0; i < globals_count; i++) {
        obj->globals[i] = ValueNull();
    }
    obj->state = ObjectModuleState_ScriptNotExecuted;

    return obj;
}

ObjectModule *ObjectModuleFromJSON(VirtualMachine *vm, ObjectString *path, const cJSON *data) {
    assert(cJSON_IsObject(data));

    ObjectString *name = RemoveExtension(vm, GetBaseName(vm, path));
    ObjectString *parent_dir = GetDirName(vm, GetDirName(vm, path));

    const cJSON *globals_count_json = cJSON_GetObjectItemCaseSensitive(data, "globals_count");
    assert(cJSON_IsNumber(globals_count_json));
    size_t globals_count = (size_t) cJSON_GetNumberValue(globals_count_json);

    const cJSON *chunk_json = cJSON_GetObjectItemCaseSensitive(data, "chunk");

    ObjectModule *module = ObjectModuleNew(vm, name, parent_dir, globals_count);
    bool put_res = HashTablePut(&vm->modules, vm, ValueObject((Object *) module->name), ValueObject((Object *) module));
    assert(put_res);

    ChunkFromJSON(&module->script->chunk, vm, module, chunk_json);

#ifdef CHUNK_DISASM_AFTER_READING
    ChunkDisassemble(&module->script->chunk, DEBUG_OUT, module->script->name->str);
#endif

    return module;
}

void ObjectModuleFree(ObjectModule *self, VirtualMachine *vm) {
    self->name = NULL;
    self->parent_dir = NULL;
    self->script = NULL;
    HashTableDeinit(&self->exports, vm);
    FREE_ARRAY(vm, self->globals, Value, self->globals_count);
    self->globals_count = 0;
    self->globals = NULL;
    FREE_OBJECT(vm, self, Module);
}

void ObjectModulePrint(const ObjectModule *self, FILE *out) {
    fprintf(out, "<module %s>", self->name->str);
}

void ObjectModuleMarkTraverse(ObjectModule *self, MemoryManager *memory) {
    ObjectMark((Object *) self->name, memory);
    ObjectMark((Object *) self->parent_dir, memory);
    ObjectMark((Object *) self->script, memory);
    HashTableMark(&self->exports, memory);
    for (size_t i = 0; i < self->globals_count; ++i) {
        ValueMark(self->globals[i], memory);
    }
}
