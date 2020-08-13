#ifndef ELET_ADDRESSFORM32_H
#define ELET_ADDRESSFORM32_H

// 32-bit addressing forms
#define AF32__EAX_PLUS_DISP8__TO__EAX (std::uint8_t)0x40
#define AF32__ECX_PLUS_DISP8__TO__EAX (std::uint8_t)0x41
#define AF32__EDX_PLUS_DISP8__TO__EAX (std::uint8_t)0x42
#define AF32__EBX_PLUS_DISP8__TO__EAX (std::uint8_t)0x43
#define AF32__SIB_PLUS_DISP8__TO__EAX (std::uint8_t)0x44
#define AF32__EBP_PLUS_DISP8__TO__EAX (std::uint8_t)0x45
#define AF32__ESI_PLUS_DISP8__TO__EAX (std::uint8_t)0x46
#define AF32__EDI_PLUS_DISP8__TO__EAX (std::uint8_t)0x47

#define AF32__EAX_PLUS_DISP8__TO__ECX (std::uint8_t)0x48
#define AF32__ECX_PLUS_DISP8__TO__ECX (std::uint8_t)0x49
#define AF32__EDX_PLUS_DISP8__TO__ECX (std::uint8_t)0x4a
#define AF32__EBX_PLUS_DISP8__TO__ECX (std::uint8_t)0x4b
#define AF32__SIB_PLUS_DISP8__TO__ECX (std::uint8_t)0x4c
#define AF32__EBP_PLUS_DISP8__TO__ECX (std::uint8_t)0x4d
#define AF32__ESI_PLUS_DISP8__TO__ECX (std::uint8_t)0x4e
#define AF32__EDI_PLUS_DISP8__TO__ECX (std::uint8_t)0x4f

#define AF32__EAX_PLUS_DISP8__TO__EDX (std::uint8_t)0x50
#define AF32__ECX_PLUS_DISP8__TO__EDX (std::uint8_t)0x51
#define AF32__EDX_PLUS_DISP8__TO__EDX (std::uint8_t)0x52
#define AF32__EBX_PLUS_DISP8__TO__EDX (std::uint8_t)0x53
#define AF32__SIB_PLUS_DISP8__TO__EDX (std::uint8_t)0x54
#define AF32__EBP_PLUS_DISP8__TO__EDX (std::uint8_t)0x55
#define AF32__ESI_PLUS_DISP8__TO__EDX (std::uint8_t)0x56
#define AF32__EDI_PLUS_DISP8__TO__EDX (std::uint8_t)0x57

#define AF32__EAX_PLUS_DISP8__TO__EBX (std::uint8_t)0x58
#define AF32__ECX_PLUS_DISP8__TO__EBX (std::uint8_t)0x59
#define AF32__EDX_PLUS_DISP8__TO__EBX (std::uint8_t)0x5a
#define AF32__EBX_PLUS_DISP8__TO__EBX (std::uint8_t)0x5b
#define AF32__SIB_PLUS_DISP8__TO__EBX (std::uint8_t)0x5c
#define AF32__EBP_PLUS_DISP8__TO__EBX (std::uint8_t)0x5d
#define AF32__ESI_PLUS_DISP8__TO__EBX (std::uint8_t)0x5e
#define AF32__EDI_PLUS_DISP8__TO__EBX (std::uint8_t)0x5f

#define AF32__EAX_PLUS_DISP8__TO__ESP (std::uint8_t)0x60
#define AF32__ECX_PLUS_DISP8__TO__ESP (std::uint8_t)0x61
#define AF32__EDX_PLUS_DISP8__TO__ESP (std::uint8_t)0x62
#define AF32__EBX_PLUS_DISP8__TO__ESP (std::uint8_t)0x63
#define AF32__SIB_PLUS_DISP8__TO__ESP (std::uint8_t)0x64
#define AF32__EBP_PLUS_DISP8__TO__ESP (std::uint8_t)0x65
#define AF32__ESI_PLUS_DISP8__TO__ESP (std::uint8_t)0x66
#define AF32__EDI_PLUS_DISP8__TO__ESP (std::uint8_t)0x67

#define AF32__EAX_PLUS_DISP8__TO__EBP (std::uint8_t)0x68
#define AF32__ECX_PLUS_DISP8__TO__EBP (std::uint8_t)0x69
#define AF32__EDX_PLUS_DISP8__TO__EBP (std::uint8_t)0x6a
#define AF32__EBX_PLUS_DISP8__TO__EBP (std::uint8_t)0x6b
#define AF32__SIB_PLUS_DISP8__TO__EBP (std::uint8_t)0x6c
#define AF32__EBP_PLUS_DISP8__TO__EBP (std::uint8_t)0x6d
#define AF32__ESI_PLUS_DISP8__TO__EBP (std::uint8_t)0x6e
#define AF32__EDI_PLUS_DISP8__TO__EBP (std::uint8_t)0x6f

#define AF32__EAX_PLUS_DISP8__TO__ESI (std::uint8_t)0x70
#define AF32__ECX_PLUS_DISP8__TO__ESI (std::uint8_t)0x71
#define AF32__EDX_PLUS_DISP8__TO__ESI (std::uint8_t)0x72
#define AF32__EBX_PLUS_DISP8__TO__ESI (std::uint8_t)0x73
#define AF32__SIB_PLUS_DISP8__TO__ESI (std::uint8_t)0x74
#define AF32__EBP_PLUS_DISP8__TO__ESI (std::uint8_t)0x75
#define AF32__ESI_PLUS_DISP8__TO__ESI (std::uint8_t)0x76
#define AF32__EDI_PLUS_DISP8__TO__ESI (std::uint8_t)0x77

#define AF32__EAX_PLUS_DISP8__TO__EDI (std::uint8_t)0x78
#define AF32__ECX_PLUS_DISP8__TO__EDI (std::uint8_t)0x79
#define AF32__EDX_PLUS_DISP8__TO__EDI (std::uint8_t)0x7a
#define AF32__EBX_PLUS_DISP8__TO__EDI (std::uint8_t)0x7b
#define AF32__SIB_PLUS_DISP8__TO__EDI (std::uint8_t)0x7c
#define AF32__EBP_PLUS_DISP8__TO__EDI (std::uint8_t)0x7d
#define AF32__ESI_PLUS_DISP8__TO__EDI (std::uint8_t)0x7e
#define AF32__EDI_PLUS_DISP8__TO__EDI (std::uint8_t)0x7f

#define AF32__EAX_PLUS_DISP32__TO__EAX (std::uint8_t)0x80
#define AF32__ECX_PLUS_DISP32__TO__EAX (std::uint8_t)0x81
#define AF32__EDX_PLUS_DISP32__TO__EAX (std::uint8_t)0x82
#define AF32__EBX_PLUS_DISP32__TO__EAX (std::uint8_t)0x83
#define AF32__SIB_PLUS_DISP32__TO__EAX (std::uint8_t)0x84
#define AF32__EBP_PLUS_DISP32__TO__EAX (std::uint8_t)0x85
#define AF32__ESI_PLUS_DISP32__TO__EAX (std::uint8_t)0x86
#define AF32__EDI_PLUS_DISP32__TO__EAX (std::uint8_t)0x87

#define AF32__EAX_PLUS_DISP32__TO__ECX (std::uint8_t)0x88
#define AF32__ECX_PLUS_DISP32__TO__ECX (std::uint8_t)0x89
#define AF32__EDX_PLUS_DISP32__TO__ECX (std::uint8_t)0x8a
#define AF32__EBX_PLUS_DISP32__TO__ECX (std::uint8_t)0x8b
#define AF32__SIB_PLUS_DISP32__TO__ECX (std::uint8_t)0x8c
#define AF32__EBP_PLUS_DISP32__TO__ECX (std::uint8_t)0x8d
#define AF32__ESI_PLUS_DISP32__TO__ECX (std::uint8_t)0x8e
#define AF32__EDI_PLUS_DISP32__TO__ECX (std::uint8_t)0x8f

#define AF32__EAX_PLUS_DISP32__TO__EDX (std::uint8_t)0x90
#define AF32__ECX_PLUS_DISP32__TO__EDX (std::uint8_t)0x91
#define AF32__EDX_PLUS_DISP32__TO__EDX (std::uint8_t)0x92
#define AF32__EBX_PLUS_DISP32__TO__EDX (std::uint8_t)0x93
#define AF32__SIB_PLUS_DISP32__TO__EDX (std::uint8_t)0x94
#define AF32__EBP_PLUS_DISP32__TO__EDX (std::uint8_t)0x95
#define AF32__ESI_PLUS_DISP32__TO__EDX (std::uint8_t)0x96
#define AF32__EDI_PLUS_DISP32__TO__EDX (std::uint8_t)0x97

#define AF32__EAX_PLUS_DISP32__TO__EBX (std::uint8_t)0x98
#define AF32__ECX_PLUS_DISP32__TO__EBX (std::uint8_t)0x99
#define AF32__EDX_PLUS_DISP32__TO__EBX (std::uint8_t)0x9a
#define AF32__EBX_PLUS_DISP32__TO__EBX (std::uint8_t)0x9b
#define AF32__SIB_PLUS_DISP32__TO__EBX (std::uint8_t)0x9c
#define AF32__EBP_PLUS_DISP32__TO__EBX (std::uint8_t)0x9d
#define AF32__ESI_PLUS_DISP32__TO__EBX (std::uint8_t)0x9e
#define AF32__EDI_PLUS_DISP32__TO__EBX (std::uint8_t)0x9f

#define AF32__EAX_PLUS_DISP32__TO__ESP (std::uint8_t)0xa0
#define AF32__ECX_PLUS_DISP32__TO__ESP (std::uint8_t)0xa1
#define AF32__EDX_PLUS_DISP32__TO__ESP (std::uint8_t)0xa2
#define AF32__EBX_PLUS_DISP32__TO__ESP (std::uint8_t)0xa3
#define AF32__SIB_PLUS_DISP32__TO__ESP (std::uint8_t)0xa4
#define AF32__EBP_PLUS_DISP32__TO__ESP (std::uint8_t)0xa5
#define AF32__ESI_PLUS_DISP32__TO__ESP (std::uint8_t)0xa6
#define AF32__EDI_PLUS_DISP32__TO__ESP (std::uint8_t)0xa7

#define AF32__EAX_PLUS_DISP32__TO__EBP (std::uint8_t)0xa8
#define AF32__ECX_PLUS_DISP32__TO__EBP (std::uint8_t)0xa9
#define AF32__EDX_PLUS_DISP32__TO__EBP (std::uint8_t)0xaa
#define AF32__EBX_PLUS_DISP32__TO__EBP (std::uint8_t)0xab
#define AF32__SIB_PLUS_DISP32__TO__EBP (std::uint8_t)0xac
#define AF32__EBP_PLUS_DISP32__TO__EBP (std::uint8_t)0xad
#define AF32__ESI_PLUS_DISP32__TO__EBP (std::uint8_t)0xae
#define AF32__EDI_PLUS_DISP32__TO__EBP (std::uint8_t)0xaf

#define AF32__EAX_PLUS_DISP32__TO__ESI (std::uint8_t)0xb0
#define AF32__ECX_PLUS_DISP32__TO__ESI (std::uint8_t)0xb1
#define AF32__EDX_PLUS_DISP32__TO__ESI (std::uint8_t)0xb2
#define AF32__EBX_PLUS_DISP32__TO__ESI (std::uint8_t)0xb3
#define AF32__SIB_PLUS_DISP32__TO__ESI (std::uint8_t)0xb4
#define AF32__EBP_PLUS_DISP32__TO__ESI (std::uint8_t)0xb5
#define AF32__ESI_PLUS_DISP32__TO__ESI (std::uint8_t)0xb6
#define AF32__EDI_PLUS_DISP32__TO__ESI (std::uint8_t)0xb7

#define AF32__EAX_PLUS_DISP32__TO__EDI (std::uint8_t)0xb8
#define AF32__ECX_PLUS_DISP32__TO__EDI (std::uint8_t)0xb9
#define AF32__EDX_PLUS_DISP32__TO__EDI (std::uint8_t)0xba
#define AF32__EBX_PLUS_DISP32__TO__EDI (std::uint8_t)0xbb
#define AF32__SIB_PLUS_DISP32__TO__EDI (std::uint8_t)0xbc
#define AF32__EBP_PLUS_DISP32__TO__EDI (std::uint8_t)0xbd
#define AF32__ESI_PLUS_DISP32__TO__EDI (std::uint8_t)0xbe
#define AF32__EDI_PLUS_DISP32__TO__EDI (std::uint8_t)0xbf

#define AF32__EAX__TO__EAX  (std::uint8_t)0xc0
#define AF32__ECX__TO__EAX  (std::uint8_t)0xc1
#define AF32__EDX__TO__EAX  (std::uint8_t)0xc2
#define AF32__EBX__TO__EAX  (std::uint8_t)0xc3
#define AF32__ESP__TO__EAX  (std::uint8_t)0xc4
#define AF32__EBP__TO__EAX  (std::uint8_t)0xc5
#define AF32__ESI__TO__EAX  (std::uint8_t)0xc6
#define AF32__EDI__TO__EAX  (std::uint8_t)0xc7

#define AF32__EAX__TO__ECX  (std::uint8_t)0xc8
#define AF32__ECX__TO__ECX  (std::uint8_t)0xc9
#define AF32__EDX__TO__ECX  (std::uint8_t)0xca
#define AF32__EBX__TO__ECX  (std::uint8_t)0xcb
#define AF32__ESP__TO__ECX  (std::uint8_t)0xcc
#define AF32__EBP__TO__ECX  (std::uint8_t)0xcd
#define AF32__ESI__TO__ECX  (std::uint8_t)0xce
#define AF32__EDI__TO__ECX  (std::uint8_t)0xcf

#define AF32__EAX__TO__EDX  (std::uint8_t)0xd0
#define AF32__ECX__TO__EDX  (std::uint8_t)0xd1
#define AF32__EDX__TO__EDX  (std::uint8_t)0xd2
#define AF32__EBX__TO__EDX  (std::uint8_t)0xd3
#define AF32__ESP__TO__EDX  (std::uint8_t)0xd4
#define AF32__EBP__TO__EDX  (std::uint8_t)0xd5
#define AF32__ESI__TO__EDX  (std::uint8_t)0xd6
#define AF32__EDI__TO__EDX  (std::uint8_t)0xd7

#define AF32__EAX__TO__EBX  (std::uint8_t)0xd8
#define AF32__ECX__TO__EBX  (std::uint8_t)0xd9
#define AF32__EDX__TO__EBX  (std::uint8_t)0xda
#define AF32__EBX__TO__EBX  (std::uint8_t)0xdb
#define AF32__ESP__TO__EBX  (std::uint8_t)0xdc
#define AF32__EBP__TO__EBX  (std::uint8_t)0xdd
#define AF32__ESI__TO__EBX  (std::uint8_t)0xde
#define AF32__EDI__TO__EBX  (std::uint8_t)0xdf

#define AF32__EAX__TO__ESP  (std::uint8_t)0xe0
#define AF32__ECX__TO__ESP  (std::uint8_t)0xe1
#define AF32__EDX__TO__ESP  (std::uint8_t)0xe2
#define AF32__EBX__TO__ESP  (std::uint8_t)0xe3
#define AF32__ESP__TO__ESP  (std::uint8_t)0xe4
#define AF32__EBP__TO__ESP  (std::uint8_t)0xe5
#define AF32__ESI__TO__ESP  (std::uint8_t)0xe6
#define AF32__EDI__TO__ESP  (std::uint8_t)0xe7

#define AF32__EAX__TO__EBP  (std::uint8_t)0xe8
#define AF32__ECX__TO__EBP  (std::uint8_t)0xe9
#define AF32__EDX__TO__EBP  (std::uint8_t)0xea
#define AF32__EBX__TO__EBP  (std::uint8_t)0xeb
#define AF32__ESP__TO__EBP  (std::uint8_t)0xec
#define AF32__EBP__TO__EBP  (std::uint8_t)0xed
#define AF32__ESI__TO__EBP  (std::uint8_t)0xee
#define AF32__EDI__TO__EBP  (std::uint8_t)0xef



#endif //ELET_ADDRESSFORM32_H
