#ifndef ELET_COMPILERTYPES_H
#define ELET_COMPILERTYPES_H


#include <format>
#include <filesystem>
#include "Syntax/Syntax.h"


namespace fs = std::filesystem;


namespace elet::domain::compiler
{
    enum class RegisterSize : uint8_t
    {
        Byte = 1,
        Word = 2,
        Dword = 4,
        Quad = 8,
        Pointer = Quad,
    };


    enum class IntegerKind : uint8_t
    {
        U8 = 1,
        S8 = 2,
        U16 = 3,
        S16 = 4,
        U32 = 5,
        S32 = 6,
        U64 = 7,
        S64 = 8,
    };


    enum class Sign
    {
        Unsigned,
        Signed,
    };


    struct IntegerType
    {
        IntegerKind
        kind;

        IntegerType(IntegerKind kind):
            kind(kind)
        { }

        RegisterSize
        size()
        {
            switch (kind)
            {
                case IntegerKind::U8:
                case IntegerKind::S8:
                    return RegisterSize::Byte;
                case IntegerKind::S16:
                case IntegerKind::U16:
                    return RegisterSize::Word;
                case IntegerKind::S32:
                case IntegerKind::U32:
                    return RegisterSize::Dword;
                case IntegerKind::S64:
                case IntegerKind::U64:
                    return RegisterSize::Quad;
            }
        }
    };


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
