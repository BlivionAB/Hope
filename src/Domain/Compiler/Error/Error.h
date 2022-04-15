#ifndef ELET_ERROR_H
#define ELET_ERROR_H


#include "Domain/Compiler/Scanner.h"
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
        SourceFile*
        sourceFile;

        Utf8String
        message;

        template<typename... Args>
        explicit CompileError(const SourceFile* sourceFile, const Syntax* syntax, std::string message, Args... args):
            syntax(syntax),
            sourceFile(sourceFile),
            message(std::format(message, args...).c_str())
        { }

        explicit CompileError(const SourceFile* sourceFile, const Syntax* syntax, std::string message):
            syntax(syntax),
            sourceFile(sourceFile),
            message(message.c_str())
        { }
    };


    struct LexicalError
    {
        const char*
        positionAddress;

        const char*
        endAddress;

        const
        SourceFile*
        sourceFile;

        Utf8String
        message;

        template<typename... Args>
        explicit LexicalError(const SourceFile* sourceFile, const char* position, const char* message, Args... args):
            sourceFile(sourceFile),
            positionAddress(position),
            endAddress(nullptr),
            message(std::format(message, args...).c_str())
        { }


        template<typename... Args>
        explicit LexicalError(const SourceFile* sourceFile, const char* position, const char* endAddress, const char* message, Args... args):
            sourceFile(sourceFile),
            positionAddress(position),
            endAddress(endAddress),
            message(std::format(message, args...).c_str())
        { }
    };


    struct SyntaxError : CompileError
    {
        template<typename... Args>
        SyntaxError(
            const SourceFile* sourceFile,
            const Syntax* syntax,
            std::string message,
            Args... args):

            CompileError(
                sourceFile,
                syntax,
                message,
                args ...)
        { }
    };


    struct TypeCheckError : SyntaxError
    {
        TypeCheckError(const SourceFile* sourceFile, const Syntax* syntax, std::string message);

        TypeCheckError(const SourceFile* sourceFile, const Syntax* syntax, std::string message, const Type* type1);

        TypeCheckError(const SourceFile* sourceFile, const Syntax* syntax, std::string message, const Type* type1, const Type* type2):
            SyntaxError(sourceFile, syntax, std::format(message, getTypeString(type1), getTypeString(type2)))
        { }

        TypeCheckError(const SourceFile* sourceFile, const Syntax* syntax, std::string message, std::string str1):
            SyntaxError(sourceFile, syntax, std::format(message, str1))
        { }

        TypeCheckError(const SourceFile* sourceFile, const Syntax* syntax, std::string message, Utf8StringView str1, const Type* type1):
            SyntaxError(sourceFile, syntax, std::format(message, std::string(str1.cStringBegin(), str1.cStringEnd()), getTypeString(type1)))
        { }

        TypeCheckError(const SourceFile* sourceFile, const Syntax* syntax, std::string message, std::string str1, std::string str2):
            SyntaxError(sourceFile, syntax, std::format(message, str1, str2))
        { }

        static
        std::string
        getTypeString(const Type* type);
    };
}


#endif //ELET_ERROR_H
