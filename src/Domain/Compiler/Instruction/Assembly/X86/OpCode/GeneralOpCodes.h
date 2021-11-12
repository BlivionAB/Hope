#ifndef ELET_GENERALOPCODES_H
#define ELET_GENERALOPCODES_H

#include "AddressForm32.h"

enum OpCodePrefix : uint8_t
{
    RexMask = 0b11110000,
    RexMagic = 0b01000000,
    RexW = 0b00001000,
    RexR = 0b00000100,
    RexX = 0b00000010,
    RexB = 0b00000001,

    OperandSizePrefix = 0x66,
    TwoByteEscape = 0x0f,
};


enum OneByteOpCode : uint8_t
{
    Push_rBX = 0x53,
    Push_rBP = 0x55,
    Push_Iz = 0x68,

    Pop_rBP = 0x5d,

    Xor_Ev_Gv = 0x31,

    JmpNear = 0xe9,
    CallNear = 0xe8,

    Lea_Gv_M = 0x8d,

    Mov_Ev_Iz = 0xc7,
    Mov_Ev_Gv = 0x89,
    Mov_Gv_Ev = 0x8b,

    Nop = 0x90,
    TwoByteNop = 0x1f,
    Ret = 0xc3,

    Sub = 0xec,
    Add_Gv_Ev = 0x03,

    // Immediate Group 1
    ImmediateGroup1_Eb_Ib = 0x80,
    ImmediateGroup1_Ev_Iz = 0x81,
    ImmediateGroup1_Eb_Ib_i64 = 0x82,
    ImmediateGroup1_Ev_Ib = 0x83,

    ExtGroup5 = 0xff,
    ExtGroup5_NearCallRegistryBits = 0b00100000,
};


enum ThreeByteOpCode
{
    Nop_0_Ev = 0x1f,
};

#define OP_NEAR_JMP_Jz  0xe9

#define OP_TWO_BYTE_PREFIX  (std::uint8_t)0x0f
#define OP_SYSCALL          (std::uint8_t)0x05


#define OFFSET_SIZE     5


#endif //ELET_GENERALOPCODES_H
