#ifndef ELET_ADDRESSFORM32_H
#define ELET_ADDRESSFORM32_H


enum ModRmMask : uint8_t
{
    Mod = 0b11000000,
    Reg = 0b00111000,
    Rm = 0b00000111
};


enum ModBits : uint8_t
{
    Mod0 = 0b00000000,
    Mod1 = 0b01000000,
    Mod2 = 0b10000000,
    Mod3 = 0b11000000,
};


enum RmBits : uint8_t
{
    Rm0 = 0b00000000,
    Rm1 = 0b00000001,
    Rm2 = 0b00000010,
    Rm3 = 0b00000011,
    Rm4 = 0b00000100,
    Rm5 = 0b00000101,
    Rm6 = 0b00000110,
    Rm7 = 0b00000111,

    Rm_RAX = Rm0,
    Rm_RCX = Rm1,
    Rm_RDX = Rm2,
    Rm_RBX = Rm3,
    Rm_RSP = Rm4,
    Rm_RBP = Rm5,
    Rm_RSI = Rm6,
    Rm_RDI = Rm7,

    EbpPlusDisp8 = Rm5,
    EbpPlusDisp32 = Rm5,

};


enum RegisterBits : uint8_t
{
    Reg0 = 0b00000000,
    Reg1 = 0b00001000,
    Reg2 = 0b00010000,
    Reg3 = 0b00011000,
    Reg4 = 0b00100000,
    Reg5 = 0b00101000,
    Reg6 = 0b00110000,
    Reg7 = 0b00111000,

    Reg_RAX = Reg0,
    Reg_RCX = Reg1,
    Reg_RDX = Reg2,
    Reg_RBX = Reg3,
    Reg_RSP = Reg4,
    Reg_RBP = Reg5,
    Reg_RSI = Reg6,
    Reg_RDI = Reg7,
};


enum Sib : uint8_t
{
    ScaleMask = 0b11000000,
    IndexMask = 0b00111000,
    BaseMask = 0b00000111,
};

#endif //ELET_ADDRESSFORM32_H
