#ifndef ELET_AARCH64INSTRUCTIONS_H
#define ELET_AARCH64INSTRUCTIONS_H


#include <variant>
#include <Foundation/List.h>
#include <Domain/Compiler/Instruction/Assembly/Aarch/AArch64Encodings.h>

namespace elet::domain::compiler::test::aarch
{
    using namespace foundation;
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


    enum class InstructionKind
    {
        Udf,
        Nop,
        LdrsbImmediateUnsignedOffset,
        LdrshImmediateUnsignedOffset,
        Ldr,
        StpOffset,
        LdpOffset,
        LdpPostIndex,
        StpPreIndex,
        StrImmediateUnsignedOffset,
        LdrImmediateUnsignedOffset,
        LdrbImmediateUnsignedOffset,
        StrbImmediateUnsignedOffset,
        LdrhImmediateUnsignedOffset,
        StrhImmediateUnsignedOffset,
        AndImmediate,
        B,
        Br,
        Bl,
        Ret,
        OrrImmediate,
        Movz,
        Movn,
        Movk,
        Adrp,
        Adr,
        Sdiv,
        Udiv,
        AddImmediate,
        SubImmediate,
        AddShiftedRegister,
        SubShiftedRegister,
        Madd,
        Msub,
        Sxtb,
        Sxth
    };


    struct Instruction
    {
        InstructionKind
        kind;

        bool
        is64Bit;

        List<uint8_t>
        bytes;

        Instruction(InstructionKind kind);

        Instruction(InstructionKind kind, bool is64Bit);
    };


    struct UdfInstruction : Instruction
    {
        int16_t
        imm16;

        UdfInstruction(int16_t imm16);
    };


    struct NopInstruction : Instruction
    {
        NopInstruction();
    };


    struct AddSubImmediateInstruction : Instruction
    {
        Register
        Rd;

        Register
        Rn;

        uint16_t
        imm12;

        AddSubImmediateInstruction(
            InstructionKind kind,
            Register Rd,
            Register Rn,
            uint16_t imm12, bool is64Bit);
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

        LdrStrImmediateUnsignedOffsetInstruction(
            InstructionKind kind,
            Register Rt,
            Register Rn,
            uint16_t imm12,
            bool is64Bit);
    };


    struct StrUnsignedOffsetInstruction : LdrStrImmediateUnsignedOffsetInstruction
    {
    };



    struct LdrUnsignedOffsetInstruction : LdrStrImmediateUnsignedOffsetInstruction
    {
    };


    struct LdrbStrbImmediateUnsignedOffsetInstruction : LdrStrImmediateUnsignedOffsetInstruction
    {
        LdrbStrbImmediateUnsignedOffsetInstruction(InstructionKind kind, Register Rt, Register Rn, uint16_t imm12);
    };


    struct LdrhStrhImmediateUnsignedOffsetInstruction : LdrbStrbImmediateUnsignedOffsetInstruction
    {
        LdrhStrhImmediateUnsignedOffsetInstruction(InstructionKind kind, Register Rt, Register Rn, uint16_t imm12);
    };


    struct LdrsbImmediateUnsignedOffsetInstruction : LdrbStrbImmediateUnsignedOffsetInstruction
    {
        LdrsbImmediateUnsignedOffsetInstruction(Register Rt, Register Rn, uint16_t imm12);
    };


    struct LdrshImmediateUnsignedOffsetInstruction : LdrbStrbImmediateUnsignedOffsetInstruction
    {
        LdrshImmediateUnsignedOffsetInstruction(Register Rt, Register Rn, uint16_t imm12);
    };


    struct LdrInstruction : Instruction
    {
        Register
        Rt;

        int32_t
        imm19;

        LdrInstruction(Register Rt, int32_t imm19, bool is64Bit);
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

        LoadStorePairInstruction(
            InstructionKind kind,
            Register Rt,
            Register Rt2,
            Register Rn,
            int8_t imm7,
            AddressMode addressMode,
            bool is64Bit);
    };


    struct AndImmediateInstruction : Instruction
    {
        uint64_t
        value;

        Register
        Rn;

        Register
        Rd;

        AndImmediateInstruction(uint64_t value, Register Rn, Register Rd, bool is64Bit);
    };


    struct BInstruction : Instruction
    {
        int32_t
        imm26;

        BInstruction(int32_t imm26);
    };


    struct BrInstruction : Instruction
    {
        Register
        Rn;

        BrInstruction(Register Rn);
    };


    struct BlInstruction : Instruction
    {
        bool
        withLink;

        int32_t
        imm26;

        BlInstruction(int32_t imm26);
    };

    struct RetInstruction : Instruction
    {

        Register
        Rn;

        RetInstruction(Register Rn);
    };


    struct OrrImmediateInstruction : Instruction
    {
        Register
        Rd;

        Register
        Rn;

        uint64_t
        value;

        OrrImmediateInstruction(Register Rd, Register Rn, uint64_t value, bool is64Bit);
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

        MovInstruction(InstructionKind kind, Register rd, uint16_t imm16, Hw hw, bool is64Bit);
    };


    struct MovzInstruction : MovInstruction
    {
        uint64_t
        immediateValue;

        MovzInstruction(Register Rd, uint16_t imm16, Hw hw, bool is64Bit);
    };


    struct MovnInstruction : MovInstruction
    {

        uint64_t
        immediateValue;

        MovnInstruction(Register rd, uint16_t imm16, Hw hw, bool is64Bit);
    };



    struct MovkInstruction : MovInstruction
    {
        uint64_t
        immediateValue;

        MovkInstruction(Register Rd, uint16_t imm16, Hw hw, bool is64Bit);
    };


    struct AdrpInstruction : Instruction
    {
        Register
        rd;

        uint32_t
        immhilo;

        AdrpInstruction(Register rd, uint32_t immhilo);
    };

    struct AdrInstruction : Instruction
    {
        Register
        rd;

        uint32_t
        immhilo;

        AdrInstruction(Register rd, uint32_t immhilo);
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

        ShiftedRegisterInstruction(
            InstructionKind instruction,
            Register Rd,
            Register Rn,
            Register Rm,
            uint8_t shift,
            uint8_t imm6,
            bool is64Bit);
    };


    struct AddShiftedRegisterInstruction : ShiftedRegisterInstruction
    {
        AddShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit);
    };


    struct SubShiftedRegisterInstruction : ShiftedRegisterInstruction
    {
        SubShiftedRegisterInstruction(Register Rd, Register Rn, Register Rm, uint8_t shift, uint8_t imm6, bool is64Bit);
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

        MaddSubInstruction(InstructionKind kind, Register Rm, Register Ra, Register Rn, Register Rd, bool is64Bit);
    };


    struct DivInstruction : Instruction
    {
        Register
        Rm;

        Register
        Rn;

        Register
        Rd;

        DivInstruction(InstructionKind kind, Register Rm, Register Rn, Register Rd, bool is64Bit);
    };


    struct SxtbSxtbhInstruction : Instruction
    {
        Register
        Rd;

        Register
        Rn;

        SxtbSxtbhInstruction(InstructionKind kind, Register Rd, Register Rn);
    };


    struct SxtbInstruction : SxtbSxtbhInstruction
    {
        SxtbInstruction(Register Rd, Register Rn);
    };


    struct SxthInstruction : SxtbSxtbhInstruction
    {
        SxthInstruction(Register Rd, Register Rn);
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

        OneOfInstruction(AddShiftedRegisterInstruction add);

        OneOfInstruction(AddSubImmediateInstruction addSubImmediate);

        OneOfInstruction(AdrInstruction adr);

        OneOfInstruction(AdrpInstruction adrp);

        OneOfInstruction(AndImmediateInstruction _and);

        OneOfInstruction(BInstruction b);

        OneOfInstruction(BlInstruction bl);

        OneOfInstruction(DivInstruction div);

        OneOfInstruction(BrInstruction br);

        OneOfInstruction(LdrInstruction ldr);

        OneOfInstruction(LdrStrImmediateUnsignedOffsetInstruction ldrstr);

        OneOfInstruction(LdrbStrbImmediateUnsignedOffsetInstruction ldrbstrb);

        OneOfInstruction(LdrhStrhImmediateUnsignedOffsetInstruction ldrhstrh);

        OneOfInstruction(LoadStorePairInstruction ldrpstrp);

        OneOfInstruction(LdrshImmediateUnsignedOffsetInstruction ldrsh);

        OneOfInstruction(LdrsbImmediateUnsignedOffsetInstruction ldrsb);

        OneOfInstruction(MaddSubInstruction maddsub);

        OneOfInstruction(MovkInstruction movk);

        OneOfInstruction(MovnInstruction movn);

        OneOfInstruction(MovzInstruction movz);

        OneOfInstruction(NopInstruction nop);

        OneOfInstruction(OrrImmediateInstruction orr);

        OneOfInstruction(RetInstruction ret);

        OneOfInstruction(SubShiftedRegisterInstruction sub);

        OneOfInstruction(SxtbInstruction sxtb);

        OneOfInstruction(SxthInstruction sxth);

        OneOfInstruction(UdfInstruction udf);


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
