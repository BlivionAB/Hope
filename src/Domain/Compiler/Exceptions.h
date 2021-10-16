#ifndef ELET_EXCEPTIONS_H
#define ELET_EXCEPTIONS_H


#include "Scanner.h"
#include <Foundation/Utf8String.h>
#include <format>
#include "Scanner.h"


namespace elet::domain::compiler::ast::error
{
    struct UnexpectedEndOfFile
    {

    };


    struct CompileError : std::exception
    {
        const
        Syntax*
        syntax;

        Utf8String
        message;

        template<typename... Args>
        explicit CompileError(const Syntax* syntax, const char* message, Args... args):
            syntax(syntax),
            message(std::format(message, args...).c_str())
        { }

        explicit CompileError(const Syntax* syntax, std::string message):
            syntax(syntax),
            message(message.c_str())
        { }
    };


    struct SyntaxError : CompileError
    {
        template<typename... Args>
        SyntaxError(const Syntax* syntax, const char* message, Args... args):
            CompileError(syntax, message, args...)
        { }
    };


    struct TypeCheckError : CompileError
    {
        template<typename... Args>
        TypeCheckError(const Syntax* syntax, std::string message):
            CompileError(syntax, message)
        { }

        template<typename... Args>
        TypeCheckError(const Syntax* syntax, const char* message, Args... args):
            CompileError(syntax, message, args...)
        { }
    };


    struct IntegerOverflowMaxLimitError : TypeCheckError
    {
        IntegerOverflowMaxLimitError(const Syntax* syntax, TypeKind type, uint64_t result):
            TypeCheckError(syntax, formatIntegerOverflowError(type, result))
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
        ExpectedTokenError(const Syntax* syntax, Token expected, Utf8StringView result):
            SyntaxError(syntax, "Expected '{0}', instead got '{1}'.", eletTokenToString.get(expected), result.toString())
        { }
    };


    struct OnlyExternCBlocksAllowedError : CompileError
    {
        OnlyExternCBlocksAllowedError(const Syntax* syntax):
            CompileError(syntax, "Only extern C blocks are allowed")
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
