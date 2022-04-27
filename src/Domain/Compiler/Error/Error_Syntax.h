#ifndef ELET_ERROR_SYNTAX_H
#define ELET_ERROR_SYNTAX_H

#include "Error.h"

namespace elet::domain::compiler::ast::error
{
    struct UnexpectedTokenError : SyntaxError
    {
        UnexpectedTokenError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            std::string expected,
            std::string current);
    };


    struct ExpectedTypeAnnotationError : SyntaxError
    {
        ExpectedTypeAnnotationError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            std::string current);
    };


    struct ExpectedExpressionError : SyntaxError
    {
        ExpectedExpressionError(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };


    struct ExpectedCDeclarationError : SyntaxError
    {
        ExpectedCDeclarationError(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };


    struct UnknownFileLevelStatement : SyntaxError
    {
        UnknownFileLevelStatement(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };


    struct UnknownDomainLevelStatement : SyntaxError
    {
        UnknownDomainLevelStatement(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };


    struct UnknownFunctionLevelStatement : SyntaxError
    {
        UnknownFunctionLevelStatement(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };


    struct IntegerLiteralOverflowError : SyntaxError
    {
        IntegerLiteralOverflowError(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };


    struct IntegerLiteralUnderflowError : SyntaxError
    {
        IntegerLiteralUnderflowError(
            const SourceFile* sourceFile,
            const Syntax* syntax);
    };
}

#endif //ELET_ERROR_SYNTAX_H
