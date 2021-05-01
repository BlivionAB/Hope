#ifndef ELET_EXCEPTIONS_H
#define ELET_EXCEPTIONS_H

#include "Scanner.h"
#include <Foundation/Utf8String.h>
#include <fmt/format.h>
#include "Scanner.h"
namespace elet::domain::compiler
{


struct CompilerError : std::exception
{
    Utf8String
    message;

    template<typename... Args>
    explicit CompilerError(const char* message, Args... args):
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


struct UnexpectedEndOfFile : CompilerError
{
    UnexpectedEndOfFile():
        CompilerError("Unexptected end of file.")
    { }
};


struct UndefinedDomainAccessError : CompilerError
{
    UndefinedDomainAccessError():
        CompilerError("Could not get access domain.")
    { }
};


struct UnknownFileLevelStatement : CompilerError
{
    UnknownFileLevelStatement():
        CompilerError("Unknown file level statement.")
    { }
};


struct UnrecognizedDeclaration : CompilerError
{
    UnrecognizedDeclaration():
        CompilerError("Cannot recognize declaration.")
    { }
};



struct UnrecognizedPrimitiveType : CompilerError
{
    UnrecognizedPrimitiveType():
        CompilerError("Cannot recognize declaration.")
    { }
};

struct UnrecognizedBindingStatement : CompilerError
{
    UnrecognizedBindingStatement():
        CompilerError("Cannot recognize binding statement.")
    { }
};



struct UnrecognizedLocalStatement : CompilerError
{
    UnrecognizedLocalStatement():
        CompilerError("Cannot recognize binding statement.")
    { }
};


}


#endif //ELET_EXCEPTIONS_H
