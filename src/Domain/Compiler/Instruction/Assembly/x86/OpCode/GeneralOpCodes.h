//
// Created by Tien Heng Ho on 2020-08-06.
//

#ifndef ELET_GENERALOPCODES_H
#define ELET_GENERALOPCODES_H

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

#define OP_TWO_BYTE_PREFIX  (std::uint8_t)0x0f
#define OP_SYSCALL          (std::uint8_t)0x05

#define OP_LEA_Gv_M (std::uint8_t)(0x8d)

#define OP_CALL     (std::uint8_t)(0xe8)



#endif //ELET_GENERALOPCODES_H
