#ifndef ELET_EXCEPTIONS_H
#define ELET_EXCEPTIONS_H


#include "Scanner.h"
#include <Foundation/Utf8String.h>
#include <format>


namespace elet::domain::compiler::ast::error
{
    struct UnexpectedEndOfFile
    {

    };


    struct CompileError
    {
        const
        Syntax*
        syntax;

        const
        ast::SourceFile*
        sourceFile;

        Utf8String
        message;

        template<typename... Args>
        explicit CompileError(const Syntax* syntax, const ast::SourceFile* sourceFile, const char* message, Args... args):
            syntax(syntax),
            sourceFile(sourceFile),
            message(std::format(message, args...).c_str())
        { }

        explicit CompileError(const Syntax* syntax, const ast::SourceFile* sourceFile, std::string message):
            syntax(syntax),
            sourceFile(sourceFile),
            message(message.c_str())
        { }
    };


    struct LexicalError
    {
        const char*
        positionAddress;

        const
        ast::SourceFile*
        sourceFile;

        Utf8String
        message;

        template<typename... Args>
        explicit LexicalError(const ast::SourceFile* sourceFile, const char* position, const char* message, Args... args):
            sourceFile(sourceFile),
            positionAddress(position),
            message(std::format(message, args...).c_str())
        { }
    };


    struct SyntaxError : CompileError
    {
        template<typename... Args>
        SyntaxError(const Syntax* syntax, const ast::SourceFile* sourceFile, const char* message, Args... args):
            CompileError(syntax, sourceFile, message, args...)
        { }
    };


    struct TypeCheckError : CompileError
    {
        template<typename... Args>
        TypeCheckError(const Syntax* syntax, const ast::SourceFile* sourceFile, std::string message):
            CompileError(syntax, sourceFile, message)
        { }

        template<typename... Args>
        TypeCheckError(const Syntax* syntax, const ast::SourceFile* sourceFile, const char* message, Args... args):
            CompileError(syntax, sourceFile, message, args...)
        { }
    };


    struct IntegerOverflowMaxLimitError : TypeCheckError
    {
        IntegerOverflowMaxLimitError(const Syntax* syntax, const ast::SourceFile* sourceFile, TypeKind type, uint64_t result):
            TypeCheckError(syntax, sourceFile, formatIntegerOverflowError(type, result))
        { }

        static
        std::string
        formatIntegerOverflowError(TypeKind type, uint64_t result)
        {
            std::string typeName;
            switch (type)
            {
                case TypeKind::S64:
                    typeName = "s64";
                    break;
                default:
                    throw std::runtime_error("No implemented type in formatIntegerOverflowError.");
            }
            return std::format("Integer value exceeds the max limit of '{1}'.", typeName);
        }
    };


    struct ExpectedTokenError : SyntaxError
    {
        ExpectedTokenError(const Syntax* syntax, const ast::SourceFile* sourceFile, Token expected, Utf8StringView result):
            SyntaxError(syntax, sourceFile, "Expected '{0}', instead got '{1}'.", eletTokenToString.get(expected), result.toString())
        { }
    };


    struct OnlyExternCBlocksAllowedError : CompileError
    {
        OnlyExternCBlocksAllowedError(const Syntax* syntax, const ast::SourceFile* sourceFile):
            CompileError(syntax, sourceFile, "Only extern C blocks are allowed")
        { }
    };


    struct UnknownTypeError
    {
        Utf8String
        feedback;

        explicit UnknownTypeError(Utf8String feedback = "Unknown type"):
            feedback(feedback)
        { }
    };
}


#endif //ELET_EXCEPTIONS_H
