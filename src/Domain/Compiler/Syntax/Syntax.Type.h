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


enum TypeSize : uint64_t
{
    _8 = 1,
    _16 = 2,
    _32 = 4,
    _64 = 8,
    Dword = _32,
    Quad = _64,
    Pointer = _64,
};


enum class TypeKind
{
    // Integer types, ranked after the conversion ranking
    UInt8,
    Int8,
    Char,
    UInt,
    Int,
    UInt16,
    Int16,
    UInt32,
    Int32,
    UInt64,
    Int64,

    USize,
    Pointer,
    F32,
    F64,
    Void,
    Custom,
    Length,
};


enum TypeSizeBounds : uint64_t
{
    UInt8Max = UINT8_MAX,
    UInt16Max = UINT16_MAX,
    UInt32Max = UINT32_MAX,
    Int32Max = INT32_MAX,
    Int64Max = INT64_MAX,
    UInt64Max = UINT64_MAX,
};

enum class Signedness
{
    Unsigned,
    Signed,
};


struct Type
{
    TypeKind
    kind;

    unsigned int
    pointers = 0;

    Struct*
    struct_;

    Type(TypeKind kind):
        kind(kind)
    { }

    Type(TypeKind kind, unsigned int pointers):
        kind(kind),
        pointers(pointers)
    { }

    TypeSize
    size()
    {
        if (pointers > 0)
        {
            return TypeSize::Pointer;
        }
        switch (kind)
        {
            case TypeKind::Int:
            case TypeKind::UInt:
            case TypeKind::Int64:
            case TypeKind::UInt64:
                return TypeSize::_64;
            case TypeKind::Int32:
                return TypeSize::_32;
            case TypeKind::UInt8:
            case TypeKind::Char:
                return TypeSize::_8;
            default:
                throw std::runtime_error("Could not resolve primitive type size.");
        }
    }

    Signedness
    sign()
    {
        switch (kind)
        {
            case TypeKind::Int8:
            case TypeKind::Int16:
            case TypeKind::Int32:
            case TypeKind::Int64:
                return Signedness::Signed;
            case TypeKind::UInt8:
            case TypeKind::UInt16:
            case TypeKind::UInt32:
            case TypeKind::UInt64:
                return Signedness::Unsigned;
            default:
                throw std::runtime_error("Cannot get signedness from type.");
        }
    }
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

    bool
    hasReturnStatementOnAllBranches = false;

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
