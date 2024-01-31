#ifndef LOOP_ERROR_H
#define LOOP_ERROR_H

#define Error_LIST(o) \
    o(None) \
    o(OutOfMemory) \
    o(UnknownOpcode) \
    o(StackOverflow) \
    o(StackUnderflow) \
    o(VariableRedefinition) \
    o(UndefinedReference) \
    o(NonCallable) \
    o(WrongArgumentsCount) \
    o(IOError) \
    o(TypeMismatch) \
    o(ZeroDivision) \
    o(InvalidJSON) \
    o(FileNotFound) \
    o(OutOfRange) \
    o(CircularImport)

typedef enum Error
{
    #define Error_ENUM(name) Error_##name,

    Error_LIST(Error_ENUM)

    #undef Error_ENUM
} Error;

const char* ErrorToString(Error value);

#define TRY(expr) do { Error error = expr; if (error != Error_None) { return error; } } while (0)

#endif // LOOP_ERROR_H
