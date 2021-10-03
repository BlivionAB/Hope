#ifndef ELET_COMPILERTYPES_H
#define ELET_COMPILERTYPES_H


#include <format>


namespace elet::domain::compiler
{
    enum class RegisterSize
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
        Elf,
        Coff,
    };
}


#endif //ELET_COMPILERTYPES_H
