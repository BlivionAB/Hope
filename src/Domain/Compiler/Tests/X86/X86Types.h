#ifndef ELET_X86TYPES_H
#define ELET_X86TYPES_H


#include <variant>
#include "Foundation/List.h"


namespace elet::domain::compiler::test
{


using namespace elet::foundation;


enum class InstructionKind
{
    Unknown,
    Push,
    Pop,
    Lea,
    Mov,
    Call,
    Ret,
    Xor,
    Nop,
};


enum Register : uint8_t
{
    rAX,
    rCX,
    rDX,
    rBX,
    rSP,
    rBP,
    rSI,
    rDI,
};




struct ByteDisplacement
{
    Register
    base;

    int8_t
    displacement;

    ByteDisplacement(Register aRegister, int8_t displacement):
        base(aRegister),
        displacement(displacement)
    { }
};


struct SibDisplacement
{
    Register
    base;

    Register
    index;

    uint8_t
    scale;

    SibDisplacement(Register base, Register index, uint8_t scale):
        base(base),
        index(index),
        scale(scale)
    { }
};


struct LongDisplacement
{
    std::variant<Register, SibDisplacement*>
    base;

    std::array<uint8_t, 4>
    displacement;

    LongDisplacement(std::variant<Register, SibDisplacement*> base, std::array<uint8_t, 4> displacement):
        base(base),
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

typedef std::variant<Register, ByteDisplacement, LongDisplacement> Ev;

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


enum class SizeKind
{
    Byte,
    Word,
    Long,
    Quad,
};


struct Instruction
{
    InstructionKind
    kind;

    List<std::uint8_t>
    bytes;

    SizeKind
    size;

    Operand
    operand1;

    Operand
    operand2;

    Instruction():
        kind(InstructionKind::Unknown),
        size(SizeKind::Long)
    { }
};


}
#endif //ELET_X86TYPES_H
