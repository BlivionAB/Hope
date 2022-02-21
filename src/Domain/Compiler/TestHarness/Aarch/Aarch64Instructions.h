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


    // TODO: Used for new rewrite.
    enum class InstructionKind
    {
        Udf,
    };

    struct Instruction
    {
        Aarch64Instruction
        kind;

        bool
        is64Bit;

        List<uint8_t>
        bytes;

        Instruction(Aarch64Instruction kind):
            kind(kind),
            is64Bit(false)
        { }

        Instruction(Aarch64Instruction kind, bool is64Bit):
            kind(kind),
            is64Bit(is64Bit)
        { }
    };



    struct UdfInstruction : Instruction
    {
        int16_t
        imm16;

        UdfInstruction(int16_t imm16):
            Instruction(Aarch64Instruction::Udf),
            imm16(imm16)
        { }
    };


    struct NopInstruction : Instruction
    {
        NopInstruction():
            Instruction(Aarch64Instruction::Nop)
        { }
    };


    struct AddSubImmediateInstruction : Instruction
    {
        Register
        Rd;

        Register
        Rn;

        uint16_t
        imm12;

        AddSubImmediateInstruction(Aarch64Instruction kind, Register Rd, Register Rn, uint16_t imm12, bool is64Bit):
            Instruction(kind, is64Bit),
            Rd(Rd),
            Rn(Rn),
            imm12(imm12)
        { }
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

    struct LdrStrImmediateUnsignedOffsetInstruction : Instruction
    {
        Register
        Rt;

        Register
        Rn;

        uint16_t
        imm12;

        LdrStrImmediateUnsignedOffsetInstruction(Aarch64Instruction kind, Register Rt, Register Rn, uint16_t imm12, bool is64Bit):
            Instruction(kind, is64Bit),
            Rt(Rt),
            Rn(Rn),
            imm12(imm12)
        {

        }
    };


    struct StrUnsignedOffsetInstruction : LdrStrImmediateUnsignedOffsetInstruction
    {
    };



    struct LdrUnsignedOffsetInstruction : LdrStrImmediateUnsignedOffsetInstruction
    {
    };


    struct LdrbStrbImmediateUnsignedOffsetInstruction : LdrStrImmediateUnsignedOffsetInstruction
    {
        LdrbStrbImmediateUnsignedOffsetInstruction(Aarch64Instruction kind, Register Rt, Register Rn, uint16_t imm12):
            LdrStrImmediateUnsignedOffsetInstruction(kind, Rt, Rn, imm12, false)
        {

        }
    };


    struct LdrhStrhImmediateUnsignedOffsetInstruction : LdrbStrbImmediateUnsignedOffsetInstruction
    {
        LdrhStrhImmediateUnsignedOffsetInstruction(Aarch64Instruction kind, Register Rt, Register Rn, uint16_t imm12):
            LdrbStrbImmediateUnsignedOffsetInstruction(kind, Rt, Rn, imm12)
        {

        }
    };


    struct LdrsbImmediateUnsignedOffsetInstruction : LdrbStrbImmediateUnsignedOffsetInstruction
    {
        LdrsbImmediateUnsignedOffsetInstruction(Register Rt, Register Rn, uint16_t imm12):
            LdrbStrbImmediateUnsignedOffsetInstruction(Aarch64Instruction::LdrsbImmediateUnsignedOffset, Rt, Rn, imm12)
        { }
    };


    struct LdrshImmediateUnsignedOffsetInstruction : LdrbStrbImmediateUnsignedOffsetInstruction
    {
        LdrshImmediateUnsignedOffsetInstruction(Register Rt, Register Rn, uint16_t imm12):
            LdrbStrbImmediateUnsignedOffsetInstruction(Aarch64Instruction::LdrshImmediateUnsignedOffset, Rt, Rn, imm12)
        { }
    };


    struct LdrInstruction : Instruction
    {
        Register
        Rt;

        int32_t
        imm19;

        LdrInstruction(Register Rt, int32_t imm19, bool is64Bit):
            Instruction(Aarch64Instruction::Ldr32, is64Bit),
            Rt(Rt),
            imm19(imm19)
        { }
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
        Rt;

        Register
        Rt2;

        Register
        Rn;

        int8_t
        imm7;

        AddressMode
        addressMode;

        LoadStorePairInstruction(Aarch64Instruction kind, Register Rt, Register Rt2, Register Rn, int8_t imm7, AddressMode addressMode, bool is64Bit):
            Instruction(kind, is64Bit),
            Rt(Rt),
            Rt2(Rt2),
            Rn(Rn),
            imm7(imm7),
            addressMode(addressMode)
        {
            assert(kind == Aarch64Instruction::StpOffset64 ||
                kind == Aarch64Instruction::LdpOffset64 ||
                kind == Aarch64Instruction::LdpPostIndex64 ||
                kind == Aarch64Instruction::StpPreIndex64 && "Unknown kind");
        }
    };


    struct AndImmediateInstruction : Instruction
    {
        uint64_t
        value;

        Register
        Rn;

        Register
        Rd;

        AndImmediateInstruction(uint64_t value, Register Rn, Register Rd, bool is64Bit):
            Instruction(Aarch64Instruction::AndImmediate, is64Bit),
            value(value),
            Rn(Rn),
            Rd(Rd)
        { }
    };


    struct BInstruction : Instruction
    {
        int32_t
        imm26;

        BInstruction(int32_t imm26):
            Instruction(Aarch64Instruction::B),
            imm26(imm26)
        { }
    };


    struct BrInstruction : Instruction
    {
        Register
        Rn;

        BrInstruction(Register Rn):
            Instruction(Aarch64Instruction::Br),
            Rn(Rn)
        { }
    };

    struct RetInstruction : Instruction
    {

        Register
        Rn;

        RetInstruction(Register Rn):
            Instruction(Aarch64Instruction::Ret),
            Rn(Rn)
        { }
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
        Rd;

        Register
        Rn;

        uint64_t
        value;

        OrrImmediateInstruction(Register Rd, Register Rn, uint64_t value, bool is64Bit):
            Instruction(Aarch64Instruction::OrrImmediate, is64Bit),
            Rd(Rd),
            Rn(Rn),
            value(value)
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
        Rd;

        uint16_t
        imm16;

        Hw
        hw;

        MovInstruction(Aarch64Instruction kind, Register rd, uint16_t imm16, Hw hw, bool is64Bit):
            Instruction(kind, is64Bit),
            Rd(rd),
            imm16(imm16),
            hw(hw)
        { }
    };


    struct MovzInstruction : MovInstruction
    {
        uint64_t
        immediateValue;

        MovzInstruction(Register Rd, uint16_t imm16, Hw hw, bool is64Bit):
            MovInstruction(Aarch64Instruction::Movz, Rd, imm16, hw, is64Bit)
        {
            immediateValue = static_cast<uint64_t>(imm16) << (static_cast<uint64_t>(hw) * 16);
        }
    };


    struct MovnInstruction : MovInstruction
    {

        uint64_t
        immediateValue;

        MovnInstruction(Register rd, uint16_t imm16, Hw hw, bool is64Bit):
            MovInstruction(Aarch64Instruction::Movn, rd, imm16, hw, is64Bit)
        {
            uint16_t s = ~imm16;
            int64_t rimm16 = static_cast<int64_t>(s);
            uint64_t lsh = static_cast<uint64_t>(hw) * 16;
            uint64_t result = (rimm16 << lsh);
            if (rimm16 >= 0)
            {
                // For some reason when lsh + 16 == 64. The left shift yields -1 instead of 0. Let's guard against that.
                if (lsh != 48)
                {
                    result |= -1i64 << (lsh + 16);
                }
                result |= (1ui64 << lsh) - 1ui64;
            }
            immediateValue = result;
        }
    };




    struct MovkInstruction : MovInstruction
    {
        uint64_t
        immediateValue;

        MovkInstruction(Register Rd, uint16_t imm16, Hw hw, bool is64Bit):
            MovInstruction(Aarch64Instruction::Movk, Rd, imm16, hw, is64Bit),
            immediateValue(imm16)
        {

        }
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

        ShiftedRegisterInstruction(Aarch64Instruction instruction, Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit):
            Instruction(instruction, is64Bit),
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
        AddShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit):
            ShiftedRegisterInstruction(Aarch64Instruction::AddShiftedRegister, Rd, Rn, Rm, shift, imm6, is64Bit)
        { }
    };


    struct SubShiftedRegisterInstruction : ShiftedRegisterInstruction
    {
        SubShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit):
            ShiftedRegisterInstruction(Aarch64Instruction::SubShiftedRegister, Rd, Rn, Rm, shift, imm6, is64Bit)
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

        MaddSubInstruction(Aarch64Instruction kind, Register Rm, Register Ra, Register Rn, Register Rd, bool is64Bit):
            Instruction(kind, is64Bit),
            Rm(Rm),
            Ra(Ra),
            Rn(Rn),
            Rd(Rd)
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

        DivInstruction(Aarch64Instruction kind, Register Rm, Register Rn, Register Rd, bool is64Bit):
            Instruction(kind, is64Bit),
            Rm(Rm),
            Rn(Rn),
            Rd(Rd)
        {

        }
    };


    struct SxtbSxtbhInstruction : Instruction
    {
        Register
        Rd;

        Register
        Rn;

        SxtbSxtbhInstruction(Aarch64Instruction kind, Register Rd, Register Rn):
            Instruction(kind),
            Rd(Rd),
            Rn(Rn)
        { }
    };


    struct SxtbInstruction : SxtbSxtbhInstruction
    {
        SxtbInstruction(Register Rd, Register Rn):
            SxtbSxtbhInstruction(Aarch64Instruction::Sxtb, Rd, Rn)
        { }
    };


    struct SxthInstruction : SxtbSxtbhInstruction
    {
        SxthInstruction(Register Rd, Register Rn):
            SxtbSxtbhInstruction(Aarch64Instruction::Sxth, Rd, Rn)
        { }
    };


    union OneOfInstruction
    {
        AddShiftedRegisterInstruction add;
        AddSubImmediateInstruction addSubImmediate;
        AdrInstruction adr;
        AdrpInstruction adrp;
        AndImmediateInstruction _and;
        BInstruction b;
        BrInstruction br;
        BlInstruction bl;
        DivInstruction div;
        LdrInstruction ldr;
        LdrStrImmediateUnsignedOffsetInstruction ldrstr;
        LdrbStrbImmediateUnsignedOffsetInstruction ldrbstrb;
        LdrhStrhImmediateUnsignedOffsetInstruction ldrhstrh;
        LdrsbImmediateUnsignedOffsetInstruction ldrsb;
        LdrshImmediateUnsignedOffsetInstruction ldrsh;
        LoadStoreInstruction ldstpr;
        LoadStorePairInstruction ldrpstrp;
        MaddSubInstruction maddsub;
        MovInstruction mov;
        MovkInstruction movk;
        MovnInstruction movn;
        MovzInstruction movz;
        NopInstruction nop;
        OrrImmediateInstruction orr;
        RetInstruction ret;
        SubShiftedRegisterInstruction sub;
        SxtbInstruction sxtb;
        SxthInstruction sxth;
        UdfInstruction udf;

        OneOfInstruction(AddShiftedRegisterInstruction add)
        {
            this->add = add;
        }

        OneOfInstruction(AddSubImmediateInstruction addSubImmediate)
        {
            auto s = sizeof(AddSubImmediateInstruction);
            this->addSubImmediate = addSubImmediate;
        }

        OneOfInstruction(AdrInstruction adr)
        {
            this->adr = adr;
        }

        OneOfInstruction(AdrpInstruction adrp)
        {
            this->adrp = adrp;
        }

        OneOfInstruction(AndImmediateInstruction _and)
        {
            this->_and = _and;
        }

        OneOfInstruction(BInstruction b)
        {
            this->b = b;
        }

        OneOfInstruction(BlInstruction bl)
        {
            this->bl = bl;
        }

        OneOfInstruction(DivInstruction div)
        {
            this->div = div;
        }


        OneOfInstruction(BrInstruction br)
        {
            this->br = br;
        }

        OneOfInstruction(LdrInstruction ldr)
        {
            this->ldr = ldr;
        }

        OneOfInstruction(LdrStrImmediateUnsignedOffsetInstruction ldrstr)
        {
            this->ldrstr = ldrstr;
        }

        OneOfInstruction(LdrbStrbImmediateUnsignedOffsetInstruction ldrbstrb)
        {
            this->ldrbstrb = ldrbstrb;
        }

        OneOfInstruction(LdrhStrhImmediateUnsignedOffsetInstruction ldrhstrh)
        {
            this->ldrhstrh = ldrhstrh;
        }

        OneOfInstruction(LoadStorePairInstruction ldrpstrp)
        {
            this->ldrpstrp = ldrpstrp;
        }

        OneOfInstruction(LdrshImmediateUnsignedOffsetInstruction ldrsh)
        {
            this->ldrsh = ldrsh;
        }

        OneOfInstruction(LdrsbImmediateUnsignedOffsetInstruction ldrsb)
        {
            this->ldrsb = ldrsb;
        }

        OneOfInstruction(MaddSubInstruction maddsub)
        {
            this->maddsub = maddsub;
        }

        OneOfInstruction(MovkInstruction movk)
        {
            this->movk = movk;
        }

        OneOfInstruction(MovnInstruction movn)
        {
            this->movn = movn;
        }

        OneOfInstruction(MovzInstruction movz)
        {
            this->movz = movz;
        }

        OneOfInstruction(NopInstruction nop)
        {
            this->nop = nop;
        }

        OneOfInstruction(OrrImmediateInstruction orr)
        {
            this->orr = orr;
        }

        OneOfInstruction(RetInstruction ret)
        {
            this->ret = ret;
        }

        OneOfInstruction(SubShiftedRegisterInstruction sub)
        {
            this->sub = sub;
        }

        OneOfInstruction(SxtbInstruction sxtb)
        {
            this->sxtb = sxtb;
        }

        OneOfInstruction(SxthInstruction sxth)
        {
            this->sxth = sxth;
        }

        OneOfInstruction(UdfInstruction udf)
        {
            this->udf = udf;
        }


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
