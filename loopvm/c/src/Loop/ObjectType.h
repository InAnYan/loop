#ifndef LOOP_OBJECTTYPE_H
#define LOOP_OBJECTTYPE_H

#define ObjectType_LIST(o) \
    o(String) \
    o(Function) \
    o(Module) \
    o(Dictionary) \
    o(Class) \
    o(Instance) \
    o(BoundMethod) \
    o(Upvalue) \
    o(Closure) \
    o(List)

typedef enum ObjectType {
#define ObjectType_ENUM(name) ObjectType_##name,

    ObjectType_LIST(ObjectType_ENUM)

#undef ObjectType_ENUM
} ObjectType;

const char *ObjectTypeToString(ObjectType value);

#endif // LOOP_OBJECTTYPE_H
