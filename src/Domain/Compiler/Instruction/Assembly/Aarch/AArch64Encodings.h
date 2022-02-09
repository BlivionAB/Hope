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
        StrbImmediateUnsignedOffset = static_cast<uint32_t>(0b0011100100) << 22,
        LdrbImmediateUnsignedOffset = static_cast<uint32_t>(0b0011100101) << 22,
        LdrImmediateUnsignedOffset64 = static_cast<uint32_t>(0b1111100101) << 22,
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

        sf = static_cast<uint32_t>(1) << 31,

        Ret = static_cast<uint32_t>(0xd65f03c0) << 0,
        Bl = static_cast<uint32_t>(0b100101) << 26,
        B = static_cast<uint32_t>(0b000101) << 26,
        Br = static_cast<uint32_t>(0b11010110000111110000000000000000) << 0,
        BrMask = MASK(0, 10) | 0b11111,
        Nop = static_cast<uint32_t>(0xd503201f) << 0,

        UnconditionalBranchRegister = static_cast<uint32_t>(0b1101011) << 25,
    };
}


#endif //ELET_AARCH64ENCODINGS_H
