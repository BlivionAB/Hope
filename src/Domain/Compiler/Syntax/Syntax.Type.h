#ifndef ELET_SYNTAX_TYPE_H
#define ELET_SYNTAX_TYPE_H

#include "Syntax.h"
#include <Foundation/List.h>
#include <variant>

using namespace elet::foundation;

namespace elet::domain::compiler::ast::type
{

struct Interface;
struct Struct;


enum class TypeKind
{
    Int,
    Char,
    UInt,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    USize,
    Pointer,
    F32,
    F64,
    Void,
    Custom,
    Length,
};


struct Type
{
    TypeKind
    kind;

    unsigned int
    pointers;

    Struct*
    struct_;
};


struct Parameter
{
    Utf8StringView
    name;

    Type*
    type;
};


struct Property
{
    Utf8StringView
    name;

    Type*
    type;
};


struct Signature : Property
{
    bool
    isStartFunction;

    List<Parameter*>
    parameters;
};


struct Interface
{
    List<Signature*>
    signatures;
};


struct Struct
{
    List<Property*>
    properties;
};

}


#endif //ELET_SYNTAX_TYPE_H
