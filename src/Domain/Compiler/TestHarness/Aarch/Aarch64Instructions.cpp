#include "./Aarch64Instructions.h"

namespace elet::domain::compiler::test::aarch
{
    Instruction::Instruction(InstructionKind kind):
        kind(kind),
        is64Bit(false)
    { }


    Instruction::Instruction(InstructionKind kind, bool is64Bit):
        kind(kind),
        is64Bit(is64Bit)
    { }


    UdfInstruction::UdfInstruction(int16_t imm16):
        Instruction(InstructionKind::Udf),
        imm16(imm16)
    { }


    NopInstruction::NopInstruction():
        Instruction(InstructionKind::Nop)
    { }


    AddSubImmediateInstruction::AddSubImmediateInstruction(InstructionKind kind, Register Rd, Register Rn, uint16_t imm12, bool is64Bit):
        Instruction(kind, is64Bit),
        Rd(Rd),
        Rn(Rn),
        imm12(imm12)
    { }


    LdrStrImmediateUnsignedOffsetInstruction::LdrStrImmediateUnsignedOffsetInstruction(InstructionKind kind, Register Rt, Register Rn, uint16_t imm12, bool is64Bit):
        Instruction(kind, is64Bit),
        Rt(Rt),
        Rn(Rn),
        imm12(imm12)
    { }


    LdrbStrbImmediateUnsignedOffsetInstruction::LdrbStrbImmediateUnsignedOffsetInstruction(InstructionKind kind, Register Rt, Register Rn, uint16_t imm12):
        LdrStrImmediateUnsignedOffsetInstruction(kind, Rt, Rn, imm12, false)
    { }


    LdrhStrhImmediateUnsignedOffsetInstruction::LdrhStrhImmediateUnsignedOffsetInstruction(InstructionKind kind, Register Rt, Register Rn, uint16_t imm12):
        LdrbStrbImmediateUnsignedOffsetInstruction(kind, Rt, Rn, imm12)
    { }


    LdrsbImmediateUnsignedOffsetInstruction::LdrsbImmediateUnsignedOffsetInstruction(Register Rt, Register Rn, uint16_t imm12):
        LdrbStrbImmediateUnsignedOffsetInstruction(InstructionKind::LdrsbImmediateUnsignedOffset, Rt, Rn, imm12)
    { }


    LdrshImmediateUnsignedOffsetInstruction::LdrshImmediateUnsignedOffsetInstruction(Register Rt, Register Rn, uint16_t imm12):
        LdrbStrbImmediateUnsignedOffsetInstruction(InstructionKind::LdrshImmediateUnsignedOffset, Rt, Rn, imm12)
    { }


    LdrInstruction::LdrInstruction(Register Rt, int32_t imm19, bool is64Bit):
        Instruction(InstructionKind::Ldr, is64Bit),
        Rt(Rt),
        imm19(imm19)
    { }


    LoadStorePairInstruction::LoadStorePairInstruction(InstructionKind kind, Register Rt, Register Rt2, Register Rn, int8_t imm7, AddressMode addressMode, bool is64Bit):
        Instruction(kind, is64Bit),
        Rt(Rt),
        Rt2(Rt2),
        Rn(Rn),
        imm7(imm7),
        addressMode(addressMode)
    {
        assert(kind == InstructionKind::StpOffset ||
            kind == InstructionKind::LdpOffset ||
            kind == InstructionKind::LdpPostIndex ||
            kind == InstructionKind::StpPreIndex && "Unknown kind");
    }


    AndImmediateInstruction::AndImmediateInstruction(uint64_t value, Register Rn, Register Rd, bool is64Bit):
        Instruction(InstructionKind::AndImmediate, is64Bit),
        value(value),
        Rn(Rn),
        Rd(Rd)
    { }


    BInstruction::BInstruction(int32_t imm26):
        Instruction(InstructionKind::B),
        imm26(imm26)
    { }


    BrInstruction::BrInstruction(Register Rn):
        Instruction(InstructionKind::Br),
        Rn(Rn)
    { }


    BlInstruction::BlInstruction(int32_t imm26):
        Instruction(InstructionKind::Bl),
        withLink(true),
        imm26(imm26)
    { }


    RetInstruction::RetInstruction(Register Rn):
        Instruction(InstructionKind::Ret),
        Rn(Rn)
    { }


    OrrImmediateInstruction::OrrImmediateInstruction(Register Rd, Register Rn, uint64_t value, bool is64Bit):
        Instruction(InstructionKind::OrrImmediate, is64Bit),
        Rd(Rd),
        Rn(Rn),
        value(value)
    { }

    MovInstruction::MovInstruction(InstructionKind kind, Register rd, uint16_t imm16, Hw hw, bool is64Bit):
        Instruction(kind, is64Bit),
        Rd(rd),
        imm16(imm16),
        hw(hw)
    { }


    MovzInstruction::MovzInstruction(Register Rd, uint16_t imm16, Hw hw, bool is64Bit):
        MovInstruction(InstructionKind::Movz, Rd, imm16, hw, is64Bit)
    {
        immediateValue = static_cast<uint64_t>(imm16) << (static_cast<uint64_t>(hw) * 16);
    }


    MovnInstruction::MovnInstruction(Register rd, uint16_t imm16, Hw hw, bool is64Bit):
        MovInstruction(InstructionKind::Movn, rd, imm16, hw, is64Bit)
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


    MovkInstruction::MovkInstruction(Register Rd, uint16_t imm16, Hw hw, bool is64Bit):
        MovInstruction(InstructionKind::Movk, Rd, imm16, hw, is64Bit),
        immediateValue(imm16)
    {

    }


    AdrpInstruction::AdrpInstruction(Register rd, uint32_t immhilo):
        Instruction(InstructionKind::Adrp),
        rd(rd),
        immhilo(immhilo)
    { }


    AdrInstruction::AdrInstruction(Register rd, uint32_t immhilo):
        Instruction(InstructionKind::Adr),
        rd(rd),
        immhilo(immhilo)
    { }


    ShiftedRegisterInstruction::ShiftedRegisterInstruction(InstructionKind instruction, Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit):
        Instruction(instruction, is64Bit),
        Rd(Rd),
        Rn(Rn),
        Rm(Rm),
        shift(shift),
        imm6(imm6)
    { }


    AddShiftedRegisterInstruction::AddShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit):
        ShiftedRegisterInstruction(InstructionKind::AddShiftedRegister, Rd, Rn, Rm, shift, imm6, is64Bit)
    { }


    SubShiftedRegisterInstruction::SubShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit):
        ShiftedRegisterInstruction(InstructionKind::SubShiftedRegister, Rd, Rn, Rm, shift, imm6, is64Bit)
    { }


    MaddSubInstruction::MaddSubInstruction(InstructionKind kind, Register Rm, Register Ra, Register Rn, Register Rd, bool is64Bit):
        Instruction(kind, is64Bit),
        Rm(Rm),
        Ra(Ra),
        Rn(Rn),
        Rd(Rd)
    { }


    DivInstruction::DivInstruction(InstructionKind kind, Register Rm, Register Rn, Register Rd, bool is64Bit):
        Instruction(kind, is64Bit),
        Rm(Rm),
        Rn(Rn),
        Rd(Rd)
    { }


    SxtInstruction::SxtInstruction(InstructionKind kind, Register Rd, Register Rn):
        Instruction(kind),
        Rd(Rd),
        Rn(Rn)
    { }


    SxtbInstruction::SxtbInstruction(Register Rd, Register Rn):
        SxtInstruction(InstructionKind::Sxtb, Rd, Rn)
    { }


    SxthInstruction::SxthInstruction(Register Rd, Register Rn):
        SxtInstruction(InstructionKind::Sxth, Rd, Rn)
    { }


    SxtwInstruction::SxtwInstruction(Register Rd, Register Rn):
        SxtInstruction(InstructionKind::Sxtw, Rd, Rn)
    { }


    OneOfInstruction::OneOfInstruction(AddShiftedRegisterInstruction add)
    {
        this->add = add;
    }


    OneOfInstruction::OneOfInstruction(AddSubImmediateInstruction addSubImmediate)
    {
        this->addSubImmediate = addSubImmediate;
    }


    OneOfInstruction::OneOfInstruction(AdrInstruction adr)
    {
        this->adr = adr;
    }


    OneOfInstruction::OneOfInstruction(AdrpInstruction adrp)
    {
        this->adrp = adrp;
    }


    OneOfInstruction::OneOfInstruction(AndImmediateInstruction _and)
    {
        this->_and = _and;
    }


    OneOfInstruction::OneOfInstruction(BInstruction b)
    {
        this->b = b;
    }


    OneOfInstruction::OneOfInstruction(BlInstruction bl)
    {
        this->bl = bl;
    }


    OneOfInstruction::OneOfInstruction(DivInstruction div)
    {
        this->div = div;
    }


    OneOfInstruction::OneOfInstruction(BrInstruction br)
    {
        this->br = br;
    }


    OneOfInstruction::OneOfInstruction(LdrInstruction ldr)
    {
        this->ldr = ldr;
    }


    OneOfInstruction::OneOfInstruction(LdrStrImmediateUnsignedOffsetInstruction ldrstr)
    {
        this->ldrstr = ldrstr;
    }


    OneOfInstruction::OneOfInstruction(LdrbStrbImmediateUnsignedOffsetInstruction ldrbstrb)
    {
        this->ldrbstrb = ldrbstrb;
    }


    OneOfInstruction::OneOfInstruction(LdrhStrhImmediateUnsignedOffsetInstruction ldrhstrh)
    {
        this->ldrhstrh = ldrhstrh;
    }


    OneOfInstruction::OneOfInstruction(LoadStorePairInstruction ldrpstrp)
    {
        this->ldrpstrp = ldrpstrp;
    }


    OneOfInstruction::OneOfInstruction(LdrshImmediateUnsignedOffsetInstruction ldrsh)
    {
        this->ldrsh = ldrsh;
    }


    OneOfInstruction::OneOfInstruction(LdrsbImmediateUnsignedOffsetInstruction ldrsb)
    {
        this->ldrsb = ldrsb;
    }


    OneOfInstruction::OneOfInstruction(MaddSubInstruction maddsub)
    {
        this->maddsub = maddsub;
    }


    OneOfInstruction::OneOfInstruction(MovkInstruction movk)
    {
        this->movk = movk;
    }


    OneOfInstruction::OneOfInstruction(MovnInstruction movn)
    {
        this->movn = movn;
    }


    OneOfInstruction::OneOfInstruction(MovzInstruction movz)
    {
        this->movz = movz;
    }


    OneOfInstruction::OneOfInstruction(NopInstruction nop)
    {
        this->nop = nop;
    }


    OneOfInstruction::OneOfInstruction(OrrImmediateInstruction orr)
    {
        this->orr = orr;
    }


    OneOfInstruction::OneOfInstruction(RetInstruction ret)
    {
        this->ret = ret;
    }


    OneOfInstruction::OneOfInstruction(SubShiftedRegisterInstruction sub):
        sub(sub)
    { }


    OneOfInstruction::OneOfInstruction(SxtbInstruction sxtb):
        sxtb(sxtb)
    { }


    OneOfInstruction::OneOfInstruction(SxthInstruction sxth):
        sxth(sxth)
    { }


    OneOfInstruction::OneOfInstruction(SxtwInstruction sxtw):
        sxtw(sxtw)
    { }


    OneOfInstruction::OneOfInstruction(UdfInstruction udf)
    {
        this->udf = udf;
    }
}