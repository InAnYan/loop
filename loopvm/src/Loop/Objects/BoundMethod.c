#include "BoundMethod.h"

#include "Function.h"
#include "Instance.h"
#include "Module.h"
#include "String.h"
#include "Class.h"

ObjectBoundMethod* ObjectBoundMethodNew(VirtualMachine* vm, ObjectInstance* receiver, ObjectFunction* method)
{
    ObjectBoundMethod* obj = ALLOCATE_OBJECT(vm, BoundMethod);

    obj->receiver = receiver;
    obj->method = method;

    return obj;
}

void ObjectBoundMethodFree(ObjectBoundMethod* self, VirtualMachine* vm)
{
    self->receiver = NULL;
    self->method = NULL;
    FREE_OBJECT(vm, self, BoundMethod);
}

void ObjectBoundMethodPrint(const ObjectBoundMethod* self, FILE* out)
{
    // Has anyone seen such a long pointer access?

    fprintf(out, "<bound method %s.%s.%s>",
            self->method->module->name->str,
            self->receiver->klass->name->str,
            self->method->name->str);
}

void ObjectBoundMethodMarkTraverse(ObjectBoundMethod* self, MemoryManager* memory)
{
    ObjectMark((Object*)self->receiver, memory);
    ObjectMark((Object*)self->method, memory);
}
