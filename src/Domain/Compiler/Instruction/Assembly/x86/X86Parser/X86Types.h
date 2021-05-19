#ifndef ELET_X86TYPES_H
#define ELET_X86TYPES_H


#include <variant>
#include "Foundation/List.h"


namespace elet::domain::compiler::instruction::x86
{


using namespace elet::foundation;


enum class InstructionKind
{
    Unknown,
    Push,
    Lea,
    Mov,
    Call,
    Ret,
};


enum class Register
{
    None,
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,

    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI,
};




struct ByteDisplacement
{
    Register
    base;

    std::int8_t
    displacement;

    ByteDisplacement(Register aRegister, std::int8_t displacement):
        base(aRegister),
        displacement(displacement)
    { }
};


struct MemoryAddress32
{
    std::array<std::uint8_t, 4>
    value;

    MemoryAddress32(std::array<std::uint8_t, 4> value):
        value(value)
    { }
};


struct Immediate
{
    std::uint64_t
    value;
};


struct Jv
{
    std::array<std::uint8_t, 4>
    offset;

    Jv(std::array<std::uint8_t, 4> offset):
        offset(offset)
    {}
};


typedef std::variant<Register> Gv;

typedef std::variant<Register, ByteDisplacement> Ev;

typedef std::variant<Register, Ev*, Gv*, Jv, MemoryAddress32*> Operand;


struct ModRMByte
{
    std::uint8_t
    mod: 2,
    reg: 3,
    rm: 3;

    ModRMByte(std::uint8_t byte):
        rm(byte)
    { }
};


struct Instruction
{
    InstructionKind
    kind;

    List<std::uint8_t>
    bytes;


    bool
    isQuadWord;

    Operand
    operand1;

    Operand
    operand2;

    Instruction():
        kind(InstructionKind::Unknown)
    { }
};


}
#endif //ELET_X86TYPES_H
