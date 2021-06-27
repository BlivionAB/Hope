#ifndef ELET_AARCHTYPES_H
#define ELET_AARCHTYPES_H


#include <variant>

namespace elet::domain::compiler::test::aarch
{


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
    r0 = 0,
    r1 = 1,
    r2 = 2,
    r3 = 3,
    r4 = 4,
    r5 = 5,
    r6 = 6,
    r7 = 7,

    fp = 29,
    lr = 30,
    sp = 31,
};


typedef int8_t imm7;
typedef int16_t imm12;
typedef Register Rn;
typedef Register Rt;
typedef Register Rt2;


typedef std::variant<std::monostate, imm7, imm12, Rn, Rt, Rt2> Operand;


struct Instruction
{
    InstructionKind
    kind;

    std::optional<Register>
    rd;

    std::optional<Register>
    rt;

    std::optional<Register>
    rt2;

    std::optional<Register>
    rn;

    std::optional<int8_t>
    imm7;

    std::optional<int16_t>
    imm12;

    List<std::uint8_t>
    bytes;

    Instruction()
    { }
};


#define INSTRUCTION_MASK 0b

}


#endif //ELET_AARCHTYPES_H
