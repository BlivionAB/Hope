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
    ThreeByteEscape1 = 0x38,
    ThreeByteEscape2 = 0x3A,
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

    Mov_Eb_Ib = 0xc6,
    Mov_Ev_Iz = 0xc7,
    Mov_Ev_Gv = 0x89,
    Mov_Gv_Ev = 0x8b,
    Movzx_Gv_Eb = 0xb6,
    Movzx_Gv_Ew = 0xb7,
    Movsx_Gv_Eb = 0xbe,
    Movsx_Gv_Ew = 0xbf,

    Nop = 0x90,
    TwoByteNop = 0x1f,
    Ret = 0xc3,

    Sub = 0xec,
    Sub_Gv_Ev = 0x2b,
    Sub_Ev_Gv = 0x29,
    Add_Ev_Gv = 0x01,
    Add_Gv_Ev = 0x03,
    Imul_Gv_Ev = 0xaf,
    Cwde = 0x98,
    Cdq = 0x99,

    // Immediate Group 1
    ImmediateGroup1_Eb_Ib = 0x80,
    ImmediateGroup1_Ev_Iz = 0x81,
    ImmediateGroup1_Eb_Ib_i64 = 0x82,
    ImmediateGroup1_Ev_Ib = 0x83,


    ExtGroup3 = 0xf7,
    ExtGroup3_DivRegBits = 0b00110000,
    ExtGroup3_IdivRegBits = 0b00111000,
    ExtGroup5 = 0xff,
    ExtGroup5_NearCallRegBits = 0b00010000,

    TwoByteOpCodePrefix = 0x0f,
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
