#ifndef ELET_PARSER_ERROR_H
#define ELET_PARSER_ERROR_H


#include <Foundation/Utf8String.h>
#include <fmt/format.h>
#include "Scanner.h"
namespace elet::domain::compiler
{


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


struct OnlyExternCBlocksAllowedError : CompilerError
{
    OnlyExternCBlocksAllowedError():
       CompilerError("Only extern C blocks are allowed")
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



struct UnexpectedExpression : CompilerError
{
    UnexpectedExpression():
        CompilerError("Expected expression, instead got .")
    { }
};


struct UnexpectedModuleAccessUsage : CompilerError
{
    UnexpectedModuleAccessUsage():
        CompilerError("Expected 'identifier', '{' or '*'.")
    { }
};


struct UnexpectedEndOfFile : CompilerError
{
    UnexpectedEndOfFile():
        CompilerError("Unexptected end of file")
    { }
};


struct UnknownFileLevelStatement
{

};


}


#endif //ELET_PARSER_ERROR_H
