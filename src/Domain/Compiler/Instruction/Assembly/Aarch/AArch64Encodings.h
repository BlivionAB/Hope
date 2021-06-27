#ifndef ELET_AARCH64ENCODINGS_H
#define ELET_AARCH64ENCODINGS_H

namespace elet::domain::compiler::instruction::output
{

enum AArch64Register : uint32_t
{
    FP = 29,
    LR = 30,
    SP = 31,
};


enum Aarch64Instruction : uint32_t
{
    LoadStoreMask = static_cast<uint32_t>(0xffffffff) << 22,
    RtMask = static_cast<uint8_t>(0x1f) << 0,
    Rt2Mask = static_cast<uint32_t>(0x1f) << 10,
    RnMask = static_cast<uint32_t>(0x1f) << 5,
    Imm7Mask = static_cast<uint32_t>(0x7f) << 15,
    StpPreIndex64 = static_cast<uint32_t>(0b1010100110) << 22,
    AddImmediate64 = static_cast<uint32_t>(0b1001000100) << 22,
};


}


#endif //ELET_AARCH64ENCODINGS_H
