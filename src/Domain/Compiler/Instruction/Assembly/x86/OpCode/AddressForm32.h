#ifndef ELET_ADDRESSFORM32_H
#define ELET_ADDRESSFORM32_H

// DISP8
#define MODRM_EBP_DISP8       0b01000101

#define MOD_BITS    0b11000000
#define REG_BITS    0b00111000
#define RM_BITS     0b00000111

// Register Only
#define MODRM_EBP           0b11000101
#define MOD0_BX_PLUS_SI     0b00000000
#define MOD0_BX_PLUS_DI     0b00000001
#define MOD0_BX_PLUS_DI     0b00000001

#define MOD_DISP0   0b00000000
#define MOD_DISP8   0b01000000
#define MOD_DISP16  0b10000000
#define MOD_REG     0b11000000

#define RM0        0
#define RM1        1
#define RM2        2
#define RM3        3
#define RM4        4
#define RM5        5
#define RM6        6
#define RM7        7

#define REG_EAX               0b00000000
#define REG_ECX               0b00001000
#define REG_EDX               0b00010000
#define REG_EBX               0b00011000
#define REG_ESP               0b00100000
#define REG_EBP               0b00101000
#define REG_ESI               0b00110000
#define REG_EDI               0b00111000

#endif //ELET_ADDRESSFORM32_H
