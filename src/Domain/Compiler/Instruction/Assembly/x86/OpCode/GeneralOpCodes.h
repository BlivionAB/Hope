#ifndef ELET_GENERALOPCODES_H
#define ELET_GENERALOPCODES_H


#include "Mov.h"
#include "AddressForm32.h"

enum RexPrefix : uint8_t
{
    REX_PREFIX_MASK = 0b01110000,
    REX_PREFIX_MAGIC = 0b01000000,
    REX_PREFIX_W = 0b00001000,
    REX_PREFIX_R = 0b00000100,
    REX_PREFIX_X = 0b00000010,
    REX_PREFIX_B = 0b00000001,
};


enum OpCodeExtensionGroup1 : uint8_t
{
    Eb_lb = 0x80,
    Ev_lz = 0x81,
    Eb_lb_i64 = 0x82,
    Ev_lb = 0x83,
};


enum OpCodePrefix : uint8_t
{
    OperandSize = 0x66,
    TwoByteOpCode = 0x0f,
};


enum OpCode : uint8_t
{

    Push_rBX = 0x53,
    Push_rBP = 0x55,
    Push_Iz = 0x68,

    Pop_rBP = 0x5d,

    Xor_Eb_Gb = 0x31,

    Lea_Gv_M = 0x8d,

    Mov_Ev_Gv = 0x89,
    Mov_Gv_Ev = 0x8b,

    Nop = 0x90,
    TwoByteNop = 0x1f,
    Ret = 0xc3,


    ExtGroup5 = 0xff,
    ExtGroup5_FarCallRegistryBits = 0b00100000,
};


#define OP_NEAR_JMP_Jz  0xe9

#define OP_TWO_BYTE_PREFIX  (std::uint8_t)0x0f
#define OP_SYSCALL          (std::uint8_t)0x05


#define OP_CALL_NEAR    0xe8
#define OFFSET_SIZE     5


#endif //ELET_GENERALOPCODES_H
