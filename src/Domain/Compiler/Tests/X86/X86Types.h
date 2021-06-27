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
    Add,
    Sub,
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


struct RegisterDisplacement
{
    Register
    base;

    RegisterDisplacement(Register base):
        base(base)
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


struct ByteDisplacement
{
    std::variant<Register, SibDisplacement*>
    base;

    int8_t
    displacement;

    ByteDisplacement(std::variant<Register, SibDisplacement*> base, int8_t displacement):
        base(base),
        displacement(displacement)
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


struct Ib
{
    std::uint8_t
    offset;

    Ib(uint8_t offset):
        offset(offset)
    { }
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

typedef std::variant<Register, ByteDisplacement, LongDisplacement, RegisterDisplacement> Ev;

typedef std::variant<std::monostate, Register, Ev*, Gv*, Jv, Ib*, MemoryAddress32*> Operand;


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
