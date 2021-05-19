#ifndef ELET_GENERALOPCODES_H
#define ELET_GENERALOPCODES_H


#include "Mov.h"
#include "AddressForm32.h"

#define REX_PREFIX_MASK     (std::uint8_t)0b01110000
#define REX_PREFIX_MAGIC    (std::uint8_t)0b01000000
#define REX_PREFIX_W        (std::uint8_t)0b00001000
#define REX_PREFIX_R        (std::uint8_t)0b00000100
#define REX_PREFIX_X        (std::uint8_t)0b00000010
#define REX_PREFIX_B        (std::uint8_t)0b00000001


#define OP_PUSH_rAX 0x50
#define OP_PUSH_rCX 0x51
#define OP_PUSH_rDX 0x52
#define OP_PUSH_rBX 0x53
#define OP_PUSH_rSP 0x54
#define OP_PUSH_rBP 0x55
#define OP_PUSH_rSI 0x56
#define OP_PUSH_rDI 0x57

#define OP_POP_rBP  0x5d

#define OP_RET      0xc3
#define OP_NOP      0x90

#define OP_LEA_Gv_M 0xd3

#define OP_EXT_GROUP5 0xff
#define OP_EXT_GROUP5_FAR_CALL_REG_BITS 0b00001100

#define OP_TWO_BYTE_PREFIX  (std::uint8_t)0x0f
#define OP_SYSCALL          (std::uint8_t)0x05

#define OP_LEA_Gv_M (std::uint8_t)(0x8d)

#define OP_CALL_NEAR    0xe8
#define OFFSET_SIZE     5


#endif //ELET_GENERALOPCODES_H
