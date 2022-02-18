#ifndef ELET_AARCH64ENCODINGS_H
#define ELET_AARCH64ENCODINGS_H

namespace elet::domain::compiler::instruction::output
{
    enum Aarch64Register : uint32_t
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
        r29 = 29,
        r30 = 30,
        r31 = 31,
        fp = r29,
        lr = r30,
        sp = r31,
        zero = r31,
    };


    #define MASK(leftPad, rightPad) (((0xffffffff << rightPad) << leftPad) >> leftPad)

    enum Aarch64Instruction : uint32_t
    {
        Udf = 0,

        // Register Masks
        RtMask = static_cast<uint8_t>(0x1f) << 0,
        Rt2Mask = static_cast<uint32_t>(0x1f) << 10,
        RnMask = static_cast<uint32_t>(0x1f) << 5,
        RmMask = static_cast<uint32_t>(0x1f) << 16,
        RaMask = static_cast<uint32_t>(0x1f) << 10,
        RdMask = static_cast<uint32_t>(0x1f) << 0,
        Imm7Mask = static_cast<uint32_t>(0x7f) << 15,
        Imm12Mask = static_cast<uint32_t>(0xfff) << 10,
        Imm16Mask = MASK(11, 5),
        Imm26Mask = MASK(6, 0),
        Imm6Mask = MASK(16, 10),
        ShiftMask = MASK(8, 22),
        Mask10 = MASK(1, 10),
        Mask15 = MASK(1, 15),
        Mask16 = MASK(0, 16),
        Mask21 = MASK(0, 21),
        Mask22 = MASK(0, 22),
        Mask23 = MASK(1, 23),
        Mask24 = MASK(0, 23),
        Mask25 = MASK(0, 25),
        Mask26 = MASK(0, 26),
        AdrMask = static_cast<uint32_t>(0b10011111) << 24,
        AdrpMask = AdrMask,
        HwMask = static_cast<uint32_t>(0b11) << 21,

        // Load & Stores
        Ldr64 = static_cast<uint32_t>(0b01011000) << 24,
        Ldr32 = static_cast<uint32_t>(0b00011000) << 24,
        StrImmediateUnsignedOffset = static_cast<uint32_t>(0b1011100100) << 22,
        LdrImmediateUnsignedOffset = static_cast<uint32_t>(0b1011100101) << 22,
        StrImmediateUnsignedOffset64 = static_cast<uint32_t>(0b1111100100) << 22,
        LdrImmediateUnsignedOffset64 = static_cast<uint32_t>(0b1111100101) << 22,
        StrbImmediateUnsignedOffset = static_cast<uint32_t>(0b0011100100) << 22,
        LdrbImmediateUnsignedOffset = static_cast<uint32_t>(0b0011100101) << 22,
        LdrsbImmediateUnsignedOffset = static_cast<uint32_t>(0b0011100111) << 22,
        StrhImmediateUnsignedOffset = static_cast<uint32_t>(0b0111100100) << 22,
        LdrhImmediateUnsignedOffset = static_cast<uint32_t>(0b0111100101) << 22,
        LdrshImmediateUnsignedOffset = static_cast<uint32_t>(0b0111100111) << 22,
        StpPreIndex64 = static_cast<uint32_t>(0b1010100110) << 22,
        StpBaseOffset64 = static_cast<uint32_t>(0b1010100100) << 22,
        LdpPostIndex64 = static_cast<uint32_t>(0b1010100011) << 22,
        LdpBaseOffset64 = static_cast<uint32_t>(0b1010100101) << 22,

        // Data Processing Immediate
        Adr = static_cast<uint32_t>(0b00010000) << 24,
        Adrp = static_cast<uint32_t>(0b10010000) << 24,
        AddImmediate64 = static_cast<uint32_t>(0b1001000100) << 22,
        AddShiftedRegister = static_cast<uint32_t>(0b00001011) << 24,
        AddShiftedRegister64 = static_cast<uint32_t>(0b10001011) << 24,
        AndImmediate = static_cast<uint32_t>(0b0001001000) << 22,
        Sxtb = static_cast<uint32_t>(0b000'100110'0'000000'000111) << 10,
        Sxth = static_cast<uint32_t>(0b000'100110'0'000000'001111) << 10,
        SubShiftedRegister = static_cast<uint32_t>(0b01001011000) << 21,
        SubShiftedRegister64 = static_cast<uint32_t>(0b11001011000) << 21,
        SubImmediate64 = static_cast<uint32_t>(0b1101000100) << 22,
        Madd = static_cast<uint32_t>(0b00011011000'00000'0) << 15, // Between single quotes are Rm field
        Madd64 = static_cast<uint32_t>(0b10011011000'00000'0) << 15, // Between single quotes are Rm field
        Msub = static_cast<uint32_t>(0b00011011000'00000'1) << 15, // Between single quotes are Rm field
        Sdiv = static_cast<uint32_t>(0b0001'1010'1100'0000'0000'11) << 10,
        Udiv = static_cast<uint32_t>(0b0001'1010'1100'0000'0000'10) << 10,
        OrrImmediate = static_cast<uint32_t>(0b01100100) << 23,

        Movz = static_cast<uint32_t>(0b010100101) << 23,
        MovBitmaskImmediate = OrrImmediate,
        Movk = static_cast<uint32_t>(0b011100101) << 23,
        Movn = static_cast<uint32_t>(0b000100101) << 23,

        sf = 1ui32 << 31,
        OpcMask = 0b11ui32 << 30,
        Opc0 = 0b00ui32 << 30,
        Opc1 = 0b01ui32 << 30,

        Ret = static_cast<uint32_t>(0xd65f03c0) << 0,
        Bl = static_cast<uint32_t>(0b100101) << 26,
        B = static_cast<uint32_t>(0b000101) << 26,
        Br = static_cast<uint32_t>(0b11010110000111110000000000000000) << 0,
        BrMask = MASK(0, 10) | 0b11111,
        Nop = static_cast<uint32_t>(0xd503201f) << 0,
        UnconditionalBranchRegister = static_cast<uint32_t>(0b1101011) << 25,
    };


    enum class RootInstruction : uint32_t
    {
        Op0_Mask = static_cast<uint32_t>(0b1111) << 25,
        Op0_DataProcessing_Immediate_0 = static_cast<uint32_t>(0b1000) << 25,
        Op0_DataProcessing_Immediate_1 = static_cast<uint32_t>(0b1001) << 25,
        Op0_BranchingExceptionSystem_0 = static_cast<uint32_t>(0b1010) << 25,
        Op0_BranchingExceptionSystem_1 = static_cast<uint32_t>(0b1011) << 25,
        Op0_LoadAndStore_0 = static_cast<uint32_t>(0b0100) << 25,
        Op0_LoadAndStore_1 = static_cast<uint32_t>(0b0110) << 25,
        Op0_LoadAndStore_2 = static_cast<uint32_t>(0b1100) << 25,
        Op0_LoadAndStore_3 = static_cast<uint32_t>(0b1110) << 25,
        Op0_DataProcessing_Register_0 = static_cast<uint32_t>(0b0101) << 25,
        Op0_DataProcessing_Register_1 = static_cast<uint32_t>(0b1101) << 25,
        Op0_DataProcessing_ScalarFloatingPointAndSimd_0 = static_cast<uint32_t>(0b0111) << 25,
        Op0_DataProcessing_ScalarFloatingPointAndSimd_1 = static_cast<uint32_t>(0b1111) << 25,
    };


    inline
    uint32_t
    operator & (RootInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (RootInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class BranchingExceptionSystem : uint32_t
    {
        Op0_Mask = 0b111ui32 << 29,
        Op1_Mask = 0b1111'1111'1111'11ui32 << 12,
        Op2_Mask = 0b11111ui32,

        Op0_Grp6 = 0b110ui32 << 29,
        Op0_GrpUnconditionalBranchImmediate_0 = 0b000ui32 << 29,
        Op0_GrpUnconditionalBranchImmediate_1 = 0b100ui32 << 29,
        Op1_UnconditionalBranch_Register = 0b1000'0000'0000'00ui32 << 12,
    };


    inline
    uint32_t
    operator & (BranchingExceptionSystem&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (BranchingExceptionSystem&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class UnconditionalBranch_Register : uint32_t
    {
        Opc_Mask  = 0b1111ui32 << 21,
        Op2_Mask  = 0b11111ui32 << 16,
        Op3_Mask  = 0b111111ui32 << 10,
        Op4_Mask  = 0b11111ui32 << 0,

        Opc_Grp0 = 0b000ui32 << 21,
        Opc_Grp1 = 0b001ui32 << 21,
        Opc_Grp2 = 0b010ui32 << 21,
        Opc_Grp3 = 0b011ui32 << 21,
        Opc_Grp4 = 0b100ui32 << 21,
        Opc_Grp5 = 0b101ui32 << 21,

        Op2_True = 0b11111ui32 << 16,
        Op3_0 = 000000ui32 << 10,
        Op3_1 = 000001ui32 << 10,
        Op3_2 = 000010ui32 << 10,
        Op3_4 = 000100ui32 << 10,
        Op3_8 = 001000ui32 << 10,
        Op4_0 = 0b0000ui32 << 0,
        Op4_True = 0b11111ui32 << 0,
    };


    inline
    uint32_t
    operator & (UnconditionalBranch_Register&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (UnconditionalBranch_Register&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class UnconditionalBranch_Immediate : uint32_t
    {
        Op0_Mask = 1ui32 << 31,
        Op0_B = 0ui32 << 31,
        Op0_Bl = 1ui32 << 31,
    };


    inline
    uint32_t
    operator & (UnconditionalBranch_Immediate&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (UnconditionalBranch_Immediate&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class LoadAndStoreInstruction : uint32_t
    {
        Op0_Mask = 0b1111ui32 << 28,
        Op1_Mask = 0b1ui32 << 26,
        Op2_Mask = 0b11ui32 << 23,
        Op3_Mask = 0b111111ui32 << 16,
        Op4_Mask = 0b11ui32 << 10,

        Op0_Grp1_0 = 0b0001ui32 << 28,
        Op0_Grp1_1 = 0b0101ui32 << 28,
        Op0_Grp1_2 = 0b1001ui32 << 28,
        Op0_Grp1_3 = 0b1101ui32 << 28,

        Op2_0 = 0b00ui32 << 23,
        Op2_1 = 0b01ui32 << 23,
        Op2_2 = 0b10ui32 << 23,
        Op2_3 = 0b11ui32 << 23,

    };


    inline
    uint32_t
    operator & (LoadAndStoreInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (LoadAndStoreInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class DataProcessingRegisterInstruction : uint32_t
    {
        Op0_Mask = 0b1ui32 << 30,
        Op1_Mask = 0b1ui32 << 28,
        Op2_Mask = 0b1111ui32 << 21,
        Op3_Mask = 0b111111ui32 << 10,

        Op1_0 = 0b0ui32 << 28,
        Op1_1 = 0b1ui32 << 28,

        Op2_1xx0_0 = 0b1000ui32 << 21,
        Op2_1xx0_1 = 0b1010ui32 << 21,
        Op2_1xx0_2 = 0b1100ui32 << 21,
        Op2_1xx0_3 = 0b1110ui32 << 21,

    };


    inline
    uint32_t
    operator & (DataProcessingRegisterInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (DataProcessingRegisterInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class AddSubtractShiftedRegisterInstruction : uint32_t
    {
        sf_op_S_Mask = 0b111ui32 << 29,
        sf_op_S_Add32 = 0b000ui32 << 29,
        sf_op_S_Adds32 = 0b001ui32 << 29,
        sf_op_S_Sub32 = 0b010ui32 << 29,
        sf_op_S_Subs32 = 0b011ui32 << 29,
        sf_op_S_Add64 = 0b100ui32 << 29,
        sf_op_S_Adds64 = 0b101ui32 << 29,
        sf_op_S_Sub64 = 0b110ui32 << 29,
        sf_op_S_Subs64 = 0b111ui32 << 29,
    };


    inline
    uint32_t
    operator & (AddSubtractShiftedRegisterInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (AddSubtractShiftedRegisterInstruction&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }
}


#endif //ELET_AARCH64ENCODINGS_H
