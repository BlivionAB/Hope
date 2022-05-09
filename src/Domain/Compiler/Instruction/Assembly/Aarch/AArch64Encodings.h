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
        StpOffset64 = static_cast<uint32_t>(0b1010100100) << 22,
        LdpPostIndex64 = static_cast<uint32_t>(0b1010100011) << 22,
        LdpOffset64 = static_cast<uint32_t>(0b1010100101) << 22,

        // Data Processing Immediate
        Adr = static_cast<uint32_t>(0b00010000) << 24,
        Adrp = static_cast<uint32_t>(0b10010000) << 24,
        AddImmediate32 = static_cast<uint32_t>(0b0001000100) << 22,
        AddImmediate64 = static_cast<uint32_t>(0b1001000100) << 22,
        AddShiftedRegister = static_cast<uint32_t>(0b00001011) << 24,
        AddShiftedRegister64 = static_cast<uint32_t>(0b10001011) << 24,
        AndImmediate = static_cast<uint32_t>(0b0001001000) << 22,
        Sxtb = static_cast<uint32_t>(0b000'100110'0'000000'000111) << 10,
        Sxth = static_cast<uint32_t>(0b000'100110'0'000000'001111) << 10,
        Sxtw = static_cast<uint32_t>(0b100'100110'1'000000'011111) << 10,
        SubShiftedRegister = static_cast<uint32_t>(0b01001011000) << 21,
        SubShiftedRegister64 = static_cast<uint32_t>(0b11001011000) << 21,
        SubImmediate32 = static_cast<uint32_t>(0b0101000100) << 22,
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
        S = 1ui32 << 29,
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
        Op0_Mask                                    = 0b1111ui32 << 25,
        Reserved                                    = 0b0000ui32 << 25,
        DataProcessing_Immediate_0                  = 0b1000ui32 << 25,
        DataProcessing_Immediate_1                  = 0b1001ui32 << 25,
        BranchingExceptionSystem_0                  = 0b1010ui32 << 25,
        BranchingExceptionSystem_1                  = 0b1011ui32 << 25,
        LoadAndStore_0                              = 0b0100ui32 << 25,
        LoadAndStore_1                              = 0b0110ui32 << 25,
        LoadAndStore_2                              = 0b1100ui32 << 25,
        LoadAndStore_3                              = 0b1110ui32 << 25,
        DataProcessing_Register_0                   = 0b0101ui32 << 25,
        DataProcessing_Register_1                   = 0b1101ui32 << 25,
        DataProcessing_ScalarFloatingPointAndSimd_0 = 0b0111ui32 << 25,
        DataProcessing_ScalarFloatingPointAndSimd_1 = 0b1111ui32 << 25,
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


    enum class BranchingExceptionSystemEncoding : uint32_t
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
    operator & (BranchingExceptionSystemEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (BranchingExceptionSystemEncoding&& bits, uint32_t value)
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


    enum class LoadAndStoreEncoding : uint32_t
    {
        Op0_Mask = 0b1111ui32 << 28,
        Op1_Mask = 0b1ui32 << 26,
        Op2_Mask = 0b11ui32 << 23,
        Op3_Mask = 0b111111ui32 << 16,
        Op4_Mask = 0b11ui32 << 10,

        Op0_xx01_0 = 0b0001ui32 << 28,
        Op0_xx01_1 = 0b0101ui32 << 28,
        Op0_xx01_2 = 0b1001ui32 << 28,
        Op0_xx01_3 = 0b1101ui32 << 28,

        Op0_xx10_0 = 0b0010ui32 << 28,
        Op0_xx10_1 = 0b0110ui32 << 28,
        Op0_xx10_2 = 0b1010ui32 << 28,
        Op0_xx10_3 = 0b1110ui32 << 28,

        Op0_xx11_0 = 0b0011ui32 << 28,
        Op0_xx11_1 = 0b0111ui32 << 28,
        Op0_xx11_2 = 0b1011ui32 << 28,
        Op0_xx11_3 = 0b1111ui32 << 28,

        Op2_0 = 0b00ui32 << 23,
        Op2_1 = 0b01ui32 << 23,
        Op2_2 = 0b10ui32 << 23,
        Op2_3 = 0b11ui32 << 23,
    };


    inline
    uint32_t
    operator & (LoadAndStoreEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (LoadAndStoreEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class LoadStoreRegister_UnsignedImmediate : uint32_t
    {
        size_V_opc_Mask          = 0b1100010011ui32 << 22,
        Strb_Immediate           = 0b0000000000ui32 << 22,
        Ldrb_Immediate           = 0b0000000001ui32 << 22,
        Ldrsb_Immediate64        = 0b0000000010ui32 << 22,
        Ldrsb_Immediate32        = 0b0000000011ui32 << 22,
        Str_Immediate_SimdFp_8   = 0b0000010000ui32 << 22,
        Ldr_Immediate_SimdFp_8   = 0b0000010001ui32 << 22,
        Str_Immediate_SimdFp_128 = 0b0000010010ui32 << 22,
        Ldr_Immediate_SimdFp_128 = 0b0000010011ui32 << 22,
        Strh_Immediate           = 0b0100000000ui32 << 22,
        Ldrh_Immediate           = 0b0100000001ui32 << 22,
        Ldrsh_Immediate64        = 0b0100000010ui32 << 22,
        Ldrsh_Immediate32        = 0b0100000011ui32 << 22,
        Str_Immediate_SimdFp_16  = 0b0100010000ui32 << 22,
        Ldr_Immediate_SimdFp_16  = 0b0100010001ui32 << 22,
        Str_Immediate_32         = 0b1000000000ui32 << 22,
        Ldr_Immediate_32         = 0b1000000001ui32 << 22,
        Ldrsw_Immediate          = 0b1000000010ui32 << 22,
        Str_Immediate_SimdFp_32  = 0b1000010000ui32 << 22,
        Ldr_Immediate_SimdFp_32  = 0b1000010001ui32 << 22,
        Str_Immediate_64         = 0b1100000000ui32 << 22,
        Ldr_Immediate_64         = 0b1100000001ui32 << 22,
        Prfm_Immediate           = 0b1100000010ui32 << 22,
        Str_Immediate_SimdFp_64  = 0b1100010000ui32 << 22,
        Ldr_Immediate_SimdFp_64  = 0b1100010001ui32 << 22,
    };


    inline
    uint32_t
    operator & (LoadStoreRegister_UnsignedImmediate&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (LoadStoreRegister_UnsignedImmediate&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class DataProcessingRegisterInstruction : uint32_t
    {
        Op0_Mask = 0b1ui32 << 30,
        Op1_Mask = 0b1ui32 << 28,
        Op2_Mask = 0b1111ui32 << 21,
        Op3_Mask = 0b111111ui32 << 10,

        Op0_0 = 0b0 << 30,
        Op1_0 = 0b0ui32 << 28,
        Op1_1 = 0b1ui32 << 28,

        Op2_0110   = 0b0110ui32 << 21,

        Op2_1xx0_0 = 0b1000ui32 << 21,
        Op2_1xx0_1 = 0b1010ui32 << 21,
        Op2_1xx0_2 = 0b1100ui32 << 21,
        Op2_1xx0_3 = 0b1110ui32 << 21,

        Op2_1xxx_0 = 0b1000ui32 << 21,
        Op2_1xxx_1 = 0b1001ui32 << 21,
        Op2_1xxx_2 = 0b1010ui32 << 21,
        Op2_1xxx_3 = 0b1011ui32 << 21,
        Op2_1xxx_4 = 0b1100ui32 << 21,
        Op2_1xxx_5 = 0b1101ui32 << 21,
        Op2_1xxx_6 = 0b1110ui32 << 21,
        Op2_1xxx_7 = 0b1111ui32 << 21,
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
        Add32        = 0b000ui32 << 29,
        Adds32       = 0b001ui32 << 29,
        Sub32        = 0b010ui32 << 29,
        Subs32       = 0b011ui32 << 29,
        Add64        = 0b100ui32 << 29,
        Adds64       = 0b101ui32 << 29,
        Sub64        = 0b110ui32 << 29,
        Subs64       = 0b111ui32 << 29,
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


    enum class DataProcessingImmediateEncoding : uint32_t
    {
        Op0_Mask                    = 0b111ui32 << 23,
        PcRelAddressing_0           = 0b000ui32 << 23,
        PcRelAddressing_1           = 0b001ui32 << 23,
        AddSubtractImmediate32      = 0b010ui32 << 23,
        AddSubtractImmediateWithTag = 0b011ui32 << 23,
        LogicalImmediate            = 0b100ui32 << 23,
        MoveWideImmediate           = 0b101ui32 << 23,
        Bitfield                    = 0b110ui32 << 23,
        Extract                     = 0b111ui32 << 23,

        op                          = 0b1ui32 << 31,
    };


    inline
    uint32_t
    operator & (DataProcessingImmediateEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (DataProcessingImmediateEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class LogicalImmediateEncoding : uint32_t
    {
        sf_opc_Mask     = 0b111ui32 << 29,
        AndImmediate32  = 0b000ui32 << 29,
        OrrImmediate32  = 0b001ui32 << 29,
        EorImmediate32  = 0b010ui32 << 29,
        AndsImmediate32 = 0b011ui32 << 29,
        AndImmediate64  = 0b100ui32 << 29,
        OrrImmediate64  = 0b101ui32 << 29,
        EorImmediate64  = 0b110ui32 << 29,
        AndsImmediate64 = 0b111ui32 << 29,
    };


    inline
    uint32_t
    operator & (LogicalImmediateEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (LogicalImmediateEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class MoveWideImmediateEncoding : uint32_t
    {
        sf_opc_Mask = 0b111ui32 << 29,
        Movn32      = 0b000ui32 << 29,
        Movz32      = 0b010ui32 << 29,
        Movk32      = 0b011ui32 << 29,
        Movn64      = 0b100ui32 << 29,
        Movz64      = 0b110ui32 << 29,
        Movk64      = 0b111ui32 << 29,
    };


    inline
    uint32_t
    operator & (MoveWideImmediateEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (MoveWideImmediateEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class LoadStoreEncoding : uint32_t
    {
        opc_V_L_Mask = 0b1100010001ui32 << 22,
        Stp32        = 0b0000000000ui32 << 22,
        Ldp32        = 0b0000000001ui32 << 22,
        StpSimdFp32  = 0b0000010000ui32 << 22,
        LdpSimdFp32  = 0b0000010001ui32 << 22,
        Stgp         = 0b0100000000ui32 << 22,
        Ldpsw        = 0b0100000001ui32 << 22,
        StpSimdFp64  = 0b0100010000ui32 << 22,
        LdpSimdFp64  = 0b0100010001ui32 << 22,
        Stp64        = 0b1000000000ui32 << 22,
        Ldp64        = 0b1000000001ui32 << 22,
        StpSimdFp128 = 0b1000010000ui32 << 22,
        LdpSimdFp128 = 0b1000010001ui32 << 22,
    };


    inline
    uint32_t
    operator & (LoadStoreEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (LoadStoreEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class AddSubtract_Immediate_Encoding : uint32_t
    {
        sf_op_S_Mask     = 0b111ui32 << 29,
        Add_Immediate32  = 0b000ui32 << 29,
        Adds_Immediate32 = 0b001ui32 << 29,
        Sub_Immediate32  = 0b010ui32 << 29,
        Subs_Immediate32 = 0b011ui32 << 29,
        Add_Immediate64  = 0b100ui32 << 29,
        Adds_Immediate64 = 0b101ui32 << 29,
        Sub_Immediate64  = 0b110ui32 << 29,
        Subs_Immediate64 = 0b111ui32 << 29,
    };


    inline
    uint32_t
    operator & (AddSubtract_Immediate_Encoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (AddSubtract_Immediate_Encoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class DataProcessing3Source : uint32_t
    {
        sf_op54_op32_o0_Mask = 0b11100000111000001ui32 << 15,
        Madd32               = 0b00000000000000000ui32 << 15,
        Msub32               = 0b00000000000000001ui32 << 15,
        Madd64               = 0b10000000000000000ui32 << 15,
        Msub64               = 0b10000000000000001ui32 << 15,
    };


    inline
    uint32_t
    operator & (DataProcessing3Source&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (DataProcessing3Source&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class DataProcessing2Source : uint32_t
    {
        sf_S_Mask   = 0b101ui32 << 29,
        opcode_Mask = 0b111111ui32 << 10,
        opcode_Udiv = 0b000010ui32 << 10,
        opcode_Sdiv = 0b000011ui32 << 10,
    };


    inline
    uint32_t
    operator & (DataProcessing2Source&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (DataProcessing2Source&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class BitfieldEncoding : uint32_t
    {
        sf_opc_N_Mask   = 0b1110000001ui32 << 22,
        Sbfm32          = 0b0000000000ui32 << 22,
        Bfm32           = 0b0010000000ui32 << 22,
        Ubfm32          = 0b0100000000ui32 << 22,
        Sbfm64          = 0b1000000001ui32 << 22,
        Bfm64           = 0b1010000001ui32 << 22,
        Ubfm64          = 0b1100000001ui32 << 22,

        imms_Mask       = 0b111111ui32 << 10,
        imms_Sxtb       = 0b000111ui32 << 10,
        imms_Sxth       = 0b001111ui32 << 10,
        imms_Sxtw       = 0b011111ui32 << 10,

        immr_Mask       = 0b111111ui32 << 16,
        immr_Sxtb       = 0b000000ui32 << 16,
        immr_Sxth       = 0b000000ui32 << 16,
        immr_Sxtw       = 0b000000ui32 << 16,
    };


    inline
    uint32_t
    operator & (BitfieldEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (BitfieldEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }


    enum class ReservedEncoding : uint32_t
    {
        Op0_Mask = 0b111ui32 << 29,
        Op1_Mask = 0b111'111'111 << 16,

        Op0_Udf    = 0b000 << 29,
        Op1_Udf    = 0b00000000000 << 16,
    };


    inline
    uint32_t
    operator & (ReservedEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) & value;
    }


    inline
    bool
    operator == (ReservedEncoding&& bits, uint32_t value)
    {
        return static_cast<uint32_t>(bits) == value;
    }
}


#endif //ELET_AARCH64ENCODINGS_H
