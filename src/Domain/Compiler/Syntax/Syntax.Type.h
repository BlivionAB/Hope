#ifndef ELET_SYNTAX_TYPE_H
#define ELET_SYNTAX_TYPE_H

#include <Foundation/Int128.h>
#include "Syntax.h"
#include "../CompilerTypes.h"
#include <Foundation/List.h>
#include <variant>

using namespace elet::foundation;


namespace elet::domain::compiler::ast::type
{
    struct Interface;
    struct Struct;

    bool
    operator >= (const Int128& value1, const IntegerLimit& value2);

    bool
    operator >= (const Int128& value1, const SignedIntegerLimit& value2);

    bool
    operator <= (const Int128& value1, const IntegerLimit& value2);

    bool
    operator <= (const Int128& value1, const SignedIntegerLimit& value2);


    struct Type
    {
        TypeKind
        kind;

        Int128
        minValue;

        Int128
        maxValue;

        bool
        explicitSet;

        unsigned int
        pointers = 0;

        Struct*
        struct_;

        Type();

        Type(TypeKind kind);

        Type(TypeKind kind, unsigned int pointers);

        Type(Int128 minValue, Int128 maxValue);

        Type(TypeKind kind, Int128 minValue, Int128 maxValue);

        Type(TypeKind kind, unsigned int pointers, Int128 minValue, Int128 maxValue, bool explicitSet);

        RegisterSize
        size();

        void
        setSet(Int128 minValue, Int128 maxValue);

        void
        setSet(Type*);

        bool
        operator != (TypeKind typeKind);

        bool
        operator == (TypeKind typeKind);

        Sign
        sign();
    };


    bool
    isIntegralType(const Type* type);


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
