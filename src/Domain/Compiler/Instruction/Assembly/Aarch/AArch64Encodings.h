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
    FP = 29,
    LR = 30,
    SP = 31,
};


#define MASK(leftPad, rightPad) (((0xffffffff << rightPad) << leftPad) >> leftPad)

enum Aarch64Instruction : uint32_t
{

    // Instruction Masks

    // Operation Masks
    OpcMask = MASK(7, 21),
    Op2Mask = MASK(11, 16),
    OP3Mask = MASK(16, 10),
    Op4Mask = MASK(27,0),

    // Register Masks
    RtMask = static_cast<uint8_t>(0x1f) << 0,
    Rt2Mask = static_cast<uint32_t>(0x1f) << 10,
    RnMask = static_cast<uint32_t>(0x1f) << 5,
    RmMask = static_cast<uint32_t>(0x1f) << 16,
    RdMask = static_cast<uint32_t>(0x1f) << 0,
    Imm7Mask = static_cast<uint32_t>(0x7f) << 15,
    Imm12Mask = static_cast<uint32_t>(0xfff) << 10,
    Imm16Mask = MASK(11, 5),
    Imm26Mask = MASK(6, 0),

    Mask21 = MASK(0, 21),
    Mask22 = MASK(0, 22),
    Mask23 = MASK(0, 23),
    Mask25 = MASK(0, 25),
    Mask26 = MASK(0, 26),

    StpPreIndex64 = static_cast<uint32_t>(0b1010100110) << 22,
    StpBaseOffset64 = static_cast<uint32_t>(0b1010100100) << 22,
    LdpPostIndex64 = static_cast<uint32_t>(0b1010100011) << 22,
    AddImmediate64 = static_cast<uint32_t>(0b100100010) << 23,
    SubImmediate64 = static_cast<uint32_t>(0b1101000100) << 22,
    Movz64 = static_cast<uint32_t>(0b01010010100) << 21,
    Mov64 = static_cast<uint32_t>(0b10101010000) << 21,
    Ret = static_cast<uint32_t>(0xd65f03c0) << 0,
    Bl = static_cast<uint32_t>(0b100101) << 26,

    UnconditionalBranchRegister = static_cast<uint32_t>(0b1101011) << 25,
};


}


#endif //ELET_AARCH64ENCODINGS_H
