#include "Error_TypeCheck.h"


namespace elet::domain::compiler::ast::error
{
    TypeAssignabilityError::TypeAssignabilityError(
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


    TypeMismatchBinaryOperationError::TypeMismatchBinaryOperationError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const Type* left,
        const Type* right):

        TypeCheckError(
            sourceFile,
            syntax,
            "Type '{}' and '{}' are not binary operable.",
            left,
            right)
    { }


    UndefinedBinaryOperatorError::UndefinedBinaryOperatorError(
        const SourceFile* sourceFile,
        const Syntax* binaryOperator,
        Type type):

        TypeCheckError(
            sourceFile,
            binaryOperator,
            "Undefined binary operator for type '{}'.",
            type == Type::Bool ? "bool" : "integral")
    { }


    IntegralExpressionMisfitError::IntegralExpressionMisfitError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const Type* type,
        Utf8StringView expression):

        TypeCheckError(
            sourceFile,
            syntax,
            "Integral expression '{}' does not fit into integral type '{}'.",
            expression,
            type)
    { }


    IntegralExpressionOverflowError::IntegralExpressionOverflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const Type* placeholder,
        const Type* target):

        TypeCheckError(
            sourceFile,
            syntax,
            "Integral expression of type '{}' exceeds maximum value of type '{}'.",
            target,
            placeholder)
    { }


    IntegralExpressionUnderflowError::IntegralExpressionUnderflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        const Type* placeholder,
        const Type* target):

        TypeCheckError(
            sourceFile,
            syntax,
            "Integral expression of type '{}' subceeds minimum value of type '{}'.",
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
            "Value subceeds min limit of '{}'.",
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


    IntegralExpressionGlobalOverflowError::IntegralExpressionGlobalOverflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        TypeCheckError(
            sourceFile,
            syntax,
            "Integral expression exceeds global maximum limit (u64_max).")
    { }


    IntegralExpressionGlobalUnderflowError::IntegralExpressionGlobalUnderflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        TypeCheckError(
            sourceFile,
            syntax,
            "Integral expression subceeds global minimum limit (s64_min).")
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