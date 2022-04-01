//
// Created by tinganho on 2022-03-05.
//

#include "Error_TypeCheck.h"


namespace elet::domain::compiler::ast::error
{
    TypeMismatchError::TypeMismatchError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const Type* placeholder,
        const Type* target):

        TypeCheckError(
            sourceFile,
            syntax,
            "Type '{}' is not assignable to type '{}'.",
            target,
            placeholder)
    { }


    IntegralMisfitError::IntegralMisfitError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const Type* placeholder,
        const Type* target):

        TypeCheckError(
            sourceFile,
            syntax,
            "Integral type '{}' does not fit into integral type '{}'.",
            target,
            placeholder)
    { }


    IntegralLiteralUnderflowError::IntegralLiteralUnderflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        IntegerKind integerKind):

        TypeCheckError(
            sourceFile,
            syntax,
            "Value exceeds min limit of '{}'.",
            integerToString(integerKind))
    { }


    IntegralLiteralOverflowError::IntegralLiteralOverflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        IntegerKind integerKind):

        TypeCheckError(
            sourceFile,
            syntax,
            "Value exceeds max limit of '{}'.",
            integerToString(integerKind))
    { }


    MissingImplementationError::MissingImplementationError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const DomainDeclaration* domainDeclaration):

        TypeCheckError(
            sourceFile,
            syntax,
            "Missing implementation of '{}'.",
            domainDeclaration->implementsClause->name.toString())
    { }
}