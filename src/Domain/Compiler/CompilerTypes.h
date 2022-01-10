#ifndef ELET_COMPILERTYPES_H
#define ELET_COMPILERTYPES_H


#include <format>
#include <filesystem>
#include "Syntax/Syntax.h"


namespace fs = std::filesystem;


namespace elet::domain::compiler
{
    enum class TypeKind
    {
        Error,

        // Integer types, ranked after the conversion ranking
        U8,
        S8,
        Char,
        U16,
        S16,
        U32,
        S32,
        U64,
        S64,
        UInt,
        Int,

        USize,
        Pointer,
        F32,
        F64,
        Void,
        Custom,
        Length,
    };


    enum class RegisterSize : uint8_t
    {
        None,
        Byte = 1,
        Word = 2,
        Dword = 4,
        Quad = 8,
        Pointer = Quad,
    };


    enum class RegisterBitSize
    {
        Byte = 8,
        Word = 16,
        Dword = 32,
        Quad = 64,
    };


    enum class AssemblyTarget
    {
        Unknown,
        x86_64,
        Aarch64,
        StashIR,
    };


    struct Diagnostic
    {
        Utf8String
        message;

        explicit Diagnostic(const char* message):
        message(message)
        { }

        template<typename... Args>
        explicit Diagnostic(const char* message, Args... args):
            message(std::format(message, args...).c_str())
        { }
    };


    enum class ObjectFileTarget
    {
        Unknown,
        StashIR,
        MachO,
        Pe32,
    };
}


#endif //ELET_COMPILERTYPES_H
