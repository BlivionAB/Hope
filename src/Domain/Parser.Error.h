#ifndef ELET_PARSER_ERROR_H
#define ELET_PARSER_ERROR_H


#include <Foundation/Utf8String.h>
#include <fmt/format.h>
#include "Scanner.h"

struct CompilerError
{
    Utf8String
    message;

    template<typename... Args>
    CompilerError(const char* message, Args... args):
        message(fmt::format(message, args...).c_str())
    { }
};

struct ExpectedTokenError : CompilerError
{
    ExpectedTokenError(Token expected, Utf8StringView result):
        CompilerError("Expected '{0}', instead got '{1}'.", eletTokenToString.get(expected), result.toString())
    { }
};

struct UnknownTypeError
{
    Utf8String
            feedback;

    UnknownTypeError(Utf8String feedback = "Unknown type"):
        feedback(feedback)
    { }
};

struct UnexpectedEndOfModule : CompilerError
{
    UnexpectedEndOfModule():
        CompilerError("Unexpected end of module.")
    { }
};


struct UnexpectedModuleAccessUsage : CompilerError
{
    UnexpectedModuleAccessUsage():
        CompilerError("Expected 'identifier', '{' or '*'.")
    { }
};
#endif //ELET_PARSER_ERROR_H
