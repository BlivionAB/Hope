#ifndef ELET_AARCH64INSTRUCTIONS_H
#define ELET_AARCH64INSTRUCTIONS_H


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
        r16 = 16,
        r17 = 17,
        r28 = 28,
        fp = 29,
        lr = 30,
        sp = 31,
        Zero = sp,
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

        bool
        is64Bit;

        List<uint8_t>
        bytes;

        Instruction(Aarch64Instruction kind):
            kind(kind)
        { }
    };



    struct UdfInstruction : Instruction
    {
        int16_t
        imm16;
    };


    struct NopInstruction : Instruction
    {

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
        Unknown,
        PreIndex,
        BaseOffset,
        PostIndex,
        UnsignedOffset,
    };

    struct LdrStrUnsignedOffsetInstruction : Instruction
    {
        Register
        Rt;

        Register
        Rn;

        uint16_t
        imm12;
    };


    struct StrUnsignedOffsetInstruction : LdrStrUnsignedOffsetInstruction
    {
    };



    struct LdrUnsignedOffsetInstruction : LdrStrUnsignedOffsetInstruction
    {
    };

    struct LdrInstruction : Instruction
    {
        Register
        rt;

        int32_t
        imm19;
    };


    struct LoadStoreInstruction : Instruction
    {
        Register
        rt;

        Register
        rn;

        int16_t
        imm12;

        AddressMode
        addressMode;
    };


    struct LoadStorePairInstruction : Instruction
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


    struct BInstruction : Instruction
    {
        int32_t
        imm26;
    };


    struct BrInstruction : Instruction
    {
        Register
        rn;
    };


    struct BlInstruction : Instruction
    {
        bool
        withLink;

        int32_t
        imm26;

        BlInstruction(int32_t imm26):
            Instruction(Aarch64Instruction::Bl),
            withLink(true),
            imm26(imm26)
        { }
    };


    struct OrrImmediateInstruction : Instruction
    {
        Register
        rd;

        Register
        rn;

        uint64_t
        immediateValue;

        OrrImmediateInstruction(Register rd, uint64_t value):
            Instruction(Aarch64Instruction::OrrImmediate),
            rd(rd),
            immediateValue(value)
        { }
    };


    enum class Hw : uint8_t
    {
        _0,
        _16,
        _32,
        _48,
    };


    struct MovInstruction : Instruction
    {
        Register
        rd;

        uint16_t
        imm16;

        Hw
        hw;

        MovInstruction(Aarch64Instruction kind, Register rd, uint16_t imm16, Hw hw):
            Instruction(kind),
            rd(rd),
            imm16(imm16),
            hw(hw)
        { }
    };


    struct MovzInstruction : MovInstruction
    {
        uint64_t
        immediateValue;

        MovzInstruction(Register rd, uint16_t imm16, Hw hw):
            MovInstruction(Aarch64Instruction::Movz, rd, imm16, hw)
        { }
    };


    struct MovnInstruction : MovInstruction
    {

        uint64_t
        immediateValue;

        MovnInstruction(Register rd, uint16_t imm16, Hw hw):
            MovInstruction(Aarch64Instruction::Movn, rd, imm16, hw)
        { }
    };




    struct MovkInstruction : MovInstruction
    {
        uint64_t
        immediateValue;

        MovkInstruction(Register rd, uint16_t imm16, Hw hw):
            MovInstruction(Aarch64Instruction::Movk, rd, imm16, hw)
        { }
    };


    struct AdrpInstruction : Instruction
    {
        Register
        rd;

        uint32_t
        immhilo;

        AdrpInstruction(Register rd, uint32_t immhilo):
            Instruction(Aarch64Instruction::Adrp),
            rd(rd),
            immhilo(immhilo)
        { }
    };

    struct AdrInstruction : Instruction
    {
        Register
        rd;

        uint32_t
        immhilo;

        AdrInstruction(Register rd, uint32_t immhilo):
            Instruction(Aarch64Instruction::Adr),
            rd(rd),
            immhilo(immhilo)
        { }
    };

    struct ShiftedRegisterInstruction : Instruction
    {
        Register
        Rd;

        Register
        Rn;

        Register
        Rm;

        uint8_t
        shift;

        uint8_t
        imm6;

        ShiftedRegisterInstruction(Aarch64Instruction instruction, Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6):
            Instruction(instruction),
            Rd(Rd),
            Rn(Rn),
            Rm(Rm),
            shift(shift),
            imm6(imm6)
        {

        }
    };


    struct AddShiftedRegisterInstruction : ShiftedRegisterInstruction
    {
        AddShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6):
            ShiftedRegisterInstruction(Aarch64Instruction::AddShiftedRegister, Rd, Rn, Rm, shift, imm6)
        { }
    };


    struct SubShiftedRegisterInstruction : ShiftedRegisterInstruction
    {
        SubShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6):
            ShiftedRegisterInstruction(Aarch64Instruction::SubShiftedRegister, Rd, Rn, Rm, shift, imm6)
        { }
    };


    struct MaddSubInstruction : Instruction
    {
        Register
        Rm;

        Register
        Ra;

        Register
        Rn;

        Register
        Rd;

        MaddSubInstruction(Aarch64Instruction kind, Register Rm, Register Ra, Register Rn, Register Rd):
            Instruction(kind),
            Rm(Rm),
            Ra(Ra),
            Rn(Rn),
            Rd(Rd)
        {

        }
    };


    struct MaddInstruction : MaddSubInstruction
    {
        MaddInstruction(Register Rm, Register Ra, Register Rn, Register Rd):
            MaddSubInstruction(Aarch64Instruction::Madd, Rm, Ra, Rn, Rd)
        {

        }
    };


    struct MsubInstruction : MaddSubInstruction
    {
        MsubInstruction(Register Rm, Register Ra, Register Rn, Register Rd):
            MaddSubInstruction(Aarch64Instruction::Msub, Rm, Ra, Rn, Rd)
        {

        }
    };


    struct DivInstruction : Instruction
    {
        Register
        Rm;

        Register
        Rn;

        Register
        Rd;

        DivInstruction(Aarch64Instruction kind, Register Rm, Register Rn, Register Rd):
            Instruction(kind),
            Rm(Rm),
            Rn(Rn),
            Rd(Rd)
        {

        }
    };


    struct SdivInstruction : DivInstruction
    {
        SdivInstruction(Register Rm, Register Rn, Register Rd):
            DivInstruction(Aarch64Instruction::Sdiv, Rm, Rn, Rd)
        {

        }
    };


    struct UdivInstruction : DivInstruction
    {
        UdivInstruction(Register Rm, Register Rn, Register Rd):
            DivInstruction(Aarch64Instruction::Udiv, Rm, Rn, Rd)
        {

        }
    };


    union OneOfInstruction
    {
        DataProcessImmediateInstruction dp;
        LoadStoreInstruction ldstpr;
        LoadStorePairInstruction lst;
        BrInstruction brexcpsysc;
        BlInstruction unbrimm;
        MovInstruction mov;
        OrrImmediateInstruction orr;
        MovnInstruction movn;
        AddShiftedRegisterInstruction add_shiftedRegister;

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
}


#endif //ELET_AARCH64INSTRUCTIONS_H
