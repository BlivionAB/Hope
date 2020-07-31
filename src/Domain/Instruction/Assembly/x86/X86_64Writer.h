#ifndef ELET_X86_64WRITER_H
#define ELET_X86_64WRITER_H

#include <queue>
#include <Domain/Instruction/Instruction.h>
#include <Domain/Instruction/Assembly/AssemblyWriterInterface.h>


namespace elet::domain::compiler::instruction::output
{


#define REX_PREFIX_MAGIC    (std::uint8_t)0b01000000
#define REX_PREFIX_W        (std::uint8_t)0b00001000
#define REX_PREFIX_R        (std::uint8_t)0b00000100
#define REX_PREFIX_X        (std::uint8_t)0b00000010
#define REX_PREFIX_B        (std::uint8_t)0b00000001

#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RAX  (std::uint8_t)0xB8
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RCX  (std::uint8_t)0xB9
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RDX  (std::uint8_t)0xBA
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RBX  (std::uint8_t)0xBB
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RSP  (std::uint8_t)0xBC
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RBP  (std::uint8_t)0xBD
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RSI  (std::uint8_t)0xBE
#define OPCODE_MOVE_IMMEDIATE_VALUE_TO_RDI  (std::uint8_t)0xBF

#define OPCODE_TWO_BYTE_PREFIX              (std::uint8_t)0x0f
#define OPCODE_SYSCALL                      (std::uint8_t)0x05

struct UnknownRegisterIndex
{

};


class X86_64Writer : public AssemblyWriterInterface
{

public:

    X86_64Writer(std::map<output::Routine*, List<std::uint8_t>*>& output, std::mutex& workMutex);

    List<std::uint8_t>*
    writeRoutine(Routine *routine);

private:
    void
    writeMoveImmediateOpcode(unsigned int registerIndex);

    List<std::uint8_t>*
    _output;
};


}


#endif //ELET_X86_64WRITER_H
