#ifndef ELET_COMPILERTYPES_H
#define ELET_COMPILERTYPES_H


#include <format>
#include <filesystem>
#include <Foundation/Utf8String.h>


namespace fs = std::filesystem;


namespace elet::domain::compiler
{
    enum class IntegerLimit : uint64_t
    {
        U8Min = 0,
        U8Max = UINT8_MAX,
        U16Min = 0,
        U16Max = UINT16_MAX,
        U32Min = 0,
        U32Max = UINT32_MAX,
        U64Min = 0,
        U64Max = UINT64_MAX,
    };


    enum class SignedIntegerLimit : int64_t
    {
        S8Max = INT8_MAX,
        S8Min = INT8_MIN,
        S16Max = INT16_MAX,
        S16Min = INT16_MIN,
        S32Max = INT32_MAX,
        S32Min = INT32_MIN,
        S64Max = INT64_MAX,
        S64Min = INT64_MIN,
    };


    enum class RegisterSize : uint8_t
    {
        None = 0,
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

    std::string
    integerToString(IntegerKind integerKind);

    enum class Sign
    {
        Mixed,
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
        // Any is assignable to anything and anything is assignable to any
        // Any is binary operable to anything.
        Any,

        // Integer types, ranked after the conversion ranking.
        // Note: US types stands for unsigned signed. Meaning it's on the unsigned
        // part of the signed type.
        ImplicitInt,
        U8,
        US8,
        S8,
        U16,
        US16,
        S16,
        U32,
        US32,
        S32,
        U64,
        US64,
        S64,
        Int,
        Uint,
        Usize,
        UndecidedInt,

        IntegralStart = U8,
        IntegralEnd = UndecidedInt,

        Char,
        Bool,

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


    enum class ObjectFileTarget
    {
        Unknown,
        StashIR,
        MachO,
        Pe32,
    };
}


#endif //ELET_COMPILERTYPES_H
