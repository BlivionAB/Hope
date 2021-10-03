#ifndef ELET_ASSEMBLY_X86_PARSER_ERROR_H
#define ELET_ASSEMBLY_X86_PARSER_ERROR_H


#include <Foundation/Utf8String.h>
#include <format>
#include "Scanner.h"

namespace elet::domain::compiler::instruction
{

struct CompilerError : std::exception
{
    Utf8String
    message;

    template<typename... Args>
    explicit CompilerError(const char* message, Args... args):
            message(std::format(message, args...).c_str())
    { }
};


struct ExpectedTokenError : CompilerError
{
    ExpectedTokenError(InstructionToken expected, Utf8StringView result):
        CompilerError("Expected '{0}', instead got '{1}'.", tokenToIRString.get(expected), result.toString())
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


}
#endif //ELET_ASSEMBLY_X86_PARSER_ERROR_H
