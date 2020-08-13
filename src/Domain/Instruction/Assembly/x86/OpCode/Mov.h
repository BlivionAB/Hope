#ifndef ELET_MOV_H
#define ELET_MOV_H

// Move immediate value to register
#define OP_MOV_rAX_Iv   (std::uint8_t)0xB8
#define OP_MOV_rCX_Iv   (std::uint8_t)0xB9
#define OP_MOV_rBX_Iv   (std::uint8_t)0xBA
#define OP_MOV_rDX_Iv   (std::uint8_t)0xBB
#define OP_MOV_rSP_Iv   (std::uint8_t)0xBC
#define OP_MOV_rBP_Iv   (std::uint8_t)0xBD
#define OP_MOV_rSI_Iv   (std::uint8_t)0xBE
#define OP_MOV_rDI_Iv   (std::uint8_t)0xBF

#define OP_MOV_Eb_Gb    (std::uint8_t)0x88
#define OP_MOV_Ev_Gv    (std::uint8_t)0x89
#define OP_MOV_Gb_Eb    (std::uint8_t)0x8a
#define OP_MOV_Gv_Ev    (std::uint8_t)0x8b

#endif //ELET_MOV_H

