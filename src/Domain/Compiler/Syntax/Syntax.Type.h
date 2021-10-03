#ifndef ELET_SYNTAX_TYPE_H
#define ELET_SYNTAX_TYPE_H

#include "Syntax.h"
#include "../CompilerTypes.h"
#include <Foundation/List.h>
#include <variant>

using namespace elet::foundation;


namespace elet::domain::compiler::ast::type
{
    struct Interface;
    struct Struct;


    enum class TypeKind
    {
        // Integer types, ranked after the conversion ranking
        U8,
        S8,
        U16,
        S16,
        U32,
        S32,
        U64,
        S64,
        Char,
        UInt,
        Int,

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

        RegisterSize
        size()
        {
            if (pointers > 0)
            {
                return RegisterSize::Pointer;
            }
            switch (kind)
            {
                case TypeKind::Int:
                case TypeKind::UInt:
                case TypeKind::S64:
                case TypeKind::U64:
                    return RegisterSize::Quad;
                case TypeKind::S32:
                    return RegisterSize::Dword;
                case TypeKind::U16:
                case TypeKind::S16:
                    return RegisterSize::Word;
                case TypeKind::U8:
                case TypeKind::S8:
                case TypeKind::Char:
                    return RegisterSize::Byte;
                default:
                    throw std::runtime_error("Could not resolve primitive type size.");
            }
        }

        Signedness
        sign()
        {
            switch (kind)
            {
                case TypeKind::S8:
                case TypeKind::S16:
                case TypeKind::S32:
                case TypeKind::S64:
                    return Signedness::Signed;
                case TypeKind::U8:
                case TypeKind::U16:
                case TypeKind::U32:
                case TypeKind::U64:
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
