#ifndef ELET_TYPECHECKERROR_H
#define ELET_TYPECHECKERROR_H


#include "Error.h"


namespace elet::domain::compiler::ast::error
{
    struct TypeAssignabilityError : TypeCheckError
    {
        TypeAssignabilityError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            const Type* placeholder,
            const Type* target);
    };


    struct TypeMismatchBinaryOperationError : TypeCheckError
    {
        TypeMismatchBinaryOperationError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            const Type* left,
            const Type* right);
    };


    struct UndefinedBinaryOperatorError : TypeCheckError
    {
        enum class Type
        {
            Integral,
            Bool,
        };

        UndefinedBinaryOperatorError(
            const SourceFile* sourceFile,
            const Syntax* binaryOperator,
            Type type);
    };


    struct IntegralExpressionMisfitError : TypeCheckError
    {
        IntegralExpressionMisfitError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            const Type* type,
            Utf8StringView expression);
    };


    struct IntegralLiteralUnderflowError : TypeCheckError
    {
        IntegralLiteralUnderflowError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            IntegerKind integerKind);
    };


    struct IntegralLiteralOverflowError : TypeCheckError
    {
        IntegralLiteralOverflowError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            IntegerKind integerKind);
    };


    struct MissingImplementationError : TypeCheckError
    {
        MissingImplementationError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            const DomainDeclaration* domainDeclaration);
    };
}


#endif //ELET_TYPECHECKERROR_H
