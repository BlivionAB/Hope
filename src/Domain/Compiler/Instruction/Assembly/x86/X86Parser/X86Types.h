#ifndef ELET_X86TYPES_H
#define ELET_X86TYPES_H


#include <variant>


namespace elet::domain::compiler::instruction::x86
{


enum class InstructionKind
{
    Push,
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

    std::uint8_t
    displacement;

    ByteDisplacement(Register aRegister, std::uint8_t displacement):
        base(aRegister),
        displacement(displacement)
    { }
};


struct Immediate
{
    std::uint64_t
    value;
};


struct Jv
{
    std::uint32_t
    offset;

    Jv(std::uint32_t offset):
        offset(offset)
    {}
};


typedef std::variant<Register> Gv;

typedef std::variant<Register, ByteDisplacement> Ev;


typedef std::variant<Register, Ev*, Gv*, Jv> Operand;


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

    bool
    is64BitOperandSize;

    Operand
    operand1;

    Operand
    operand2;
};


}
#endif //ELET_X86TYPES_H
