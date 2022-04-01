#ifndef ELET_TYPECHECKERROR_H
#define ELET_TYPECHECKERROR_H


#include "Error.h"


namespace elet::domain::compiler::ast::error
{
    struct TypeMismatchError : TypeCheckError
    {
        TypeMismatchError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            const Type* placeholder,
            const Type* target);
    };


    struct IntegralMisfitError : TypeCheckError
    {
        IntegralMisfitError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            const Type* placeholder,
            const Type* target);
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
