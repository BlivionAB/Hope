#ifndef ELET_AARCHTYPES_H
#define ELET_AARCHTYPES_H


#include <variant>
#include <Domain/Compiler/Instruction/Assembly/Aarch/AArch64Encodings.h>

namespace elet::domain::compiler::test::aarch
{


using namespace elet::domain::compiler::instruction::output;

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
    r8 = 8,
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
    Aarch64Instruction
    kind;

    List<uint8_t>
    bytes;
};


struct DataProcessImmediateInstruction : Instruction
{
    Register
    rd;

    Register
    rn;

    uint16_t
    imm12;
};


enum class AddressMode
{
    PreIndex,
    BaseOffset,
    PostIndex,
};


struct LoadStoreInstruction : Instruction
{
    Register
    rt;

    Register
    rt2;

    Register
    rn;

    int8_t
    imm7;

    AddressMode
    addressMode;
};


struct BranchExceptionSyscallInstruction : Instruction
{
    Register
    rn;
};


struct UnconditionalBranchImmediateInstruction : Instruction
{
    bool
    withLink;

    int32_t
    imm26;

    UnconditionalBranchImmediateInstruction(int32_t imm26):
        withLink(true),
        imm26(imm26)
    { }
};


struct MovWideImmediateInstruction : Instruction
{
    Register
    rd;

    uint32_t
    imm16;

    MovWideImmediateInstruction(Register rd, uint32_t imm16):
        rd(rd),
        imm16(imm16)
    { }
};


struct MovInstruction : Instruction
{
    Register
    rd;

    Register
    rm;

    MovInstruction(Register rd, Register rm):
        rd(rd),
        rm(rm)
    { }
};


union OneOfInstruction
{
    DataProcessImmediateInstruction dp;
    LoadStoreInstruction lst;
    BranchExceptionSyscallInstruction brexcpsysc;
    UnconditionalBranchImmediateInstruction unbrimm;
    MovInstruction mov;

    OneOfInstruction()
    {
        memset(this, 0, sizeof(OneOfInstruction));
        auto self = reinterpret_cast<Instruction*>(this);
        new (&self->bytes) List<uint8_t>();
    }

    ~OneOfInstruction()
    {

    }
};

#define INSTRUCTION_MASK 0b

}


#endif //ELET_AARCHTYPES_H
