#ifndef LOOP_OBJECTS_BOUNDMETHOD_H
#define LOOP_OBJECTS_BOUNDMETHOD_H

#include "../Common.h"
#include "../Object.h"

typedef struct ObjectBoundMethod
{
    Object base;
    ObjectInstance* receiver;
    ObjectFunction* method;
} ObjectBoundMethod;

ObjectBoundMethod* ObjectBoundMethodNew(VirtualMachine* vm, ObjectInstance* receiver, ObjectFunction* method);
void ObjectBoundMethodFree(ObjectBoundMethod* self, VirtualMachine* vm);

void ObjectBoundMethodPrint(const ObjectBoundMethod* self, FILE* out);

#endif // LOOP_OBJECTS_BOUNDMETHOD_H
