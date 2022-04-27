#include "Error_Syntax.h"

namespace elet::domain::compiler::ast::error
{
    UnexpectedTokenError::UnexpectedTokenError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        std::string expected,
        std::string current):

        SyntaxError(
            sourceFile,
            syntax,
            "Expected '{}', but instead got '{}'.",
            expected,
            current)
    { }


    ExpectedTypeAnnotationError::ExpectedTypeAnnotationError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        std::string current):

        SyntaxError(
            sourceFile,
            syntax,
            "Expected type annotation, but instead got '{}'.",
            current)
    { }


    ExpectedExpressionError::ExpectedExpressionError(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Expected expressions.")
    { }


    ExpectedCDeclarationError::ExpectedCDeclarationError(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Expected C declaration.")
    { }


    UnknownFileLevelStatement::UnknownFileLevelStatement(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Unknown file level statement.")
    { }


    UnknownDomainLevelStatement::UnknownDomainLevelStatement(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Unknown domain level statement.")
    { }


    UnknownFunctionLevelStatement::UnknownFunctionLevelStatement(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Unknown function level statement.")
    { }


    IntegerLiteralOverflowError::IntegerLiteralOverflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Integer literal exceeds global maximum limit (u64_max).")
    { }


    IntegerLiteralUnderflowError::IntegerLiteralUnderflowError(
        const SourceFile* sourceFile,
        const Syntax* syntax):

        SyntaxError(
            sourceFile,
            syntax,
            "Integer literal subceeds global minimum limit (s64_min).")
    { }
}