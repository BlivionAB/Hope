#ifndef ELET_ADDRESSFORM32_H
#define ELET_ADDRESSFORM32_H

// DISP8
#define MODRM_EBP_DISP8       0b01000101

#define MOD_BITS    0b11000000
#define REG_BITS    0b00111000
#define RM_BITS     0b00000111

// OpCodeRegister Only
#define MODRM_EBP           0b11000101
#define MOD0_BX_PLUS_SI     0b00000000
#define MOD0_BX_PLUS_DI     0b00000001
#define MOD0_BX_PLUS_DI     0b00000001

#define MOD_DISP0   0b00000000
#define MOD_DISP8   0b01000000
#define MOD_DISP16  0b10000000
#define MOD_REG     0b11000000

enum Mod
{
    MOD0 = 0,
    MOD1 = 1,
    MOD2 = 2,
    MOD3 = 3,
};


enum Rm : uint8_t
{
    RM0 = 0,
    RM1 = 1,
    RM2 = 2,
    RM3 = 3,
    RM4 = 4,
    RM5 = 5,
    RM6 = 6,
    RM7 = 7,
};


enum OpCodeRegister : uint8_t
{
    Reg0 = 0b00000000,
    REG1 = 0b00001000,
    REG2 = 0b00010000,
    REG3 = 0b00011000,
    REG4 = 0b00100000,
    REG5 = 0b00101000,
    REG6 = 0b00110000,
    REG7 = 0b00111000,
};

#endif //ELET_ADDRESSFORM32_H
