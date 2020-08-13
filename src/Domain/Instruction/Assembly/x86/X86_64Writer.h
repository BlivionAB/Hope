#ifndef ELET_X86_64WRITER_H
#define ELET_X86_64WRITER_H

#include <Domain/Instruction/Instruction.h>

#include <queue>

#include <Domain/Instruction/Assembly/AssemblyWriterInterface.h>
#include <Domain/Instruction/Instruction.h>
#include "OpCode/Mov.h"
#include "OpCode/GeneralOpCodes.h"
#include "OpCode/AddressForm32.h"

namespace elet::domain::compiler::instruction::output
{


struct Int32;
struct Int64;


#define DISP8_MAX_VALUE     0x100
#define DISP16_MAX_VALUE    0x10000
#define DISP32_MAX_VALUE    0x100000000

#define REX_PREFIX_MAGIC    (std::uint8_t)0b01000000
#define REX_PREFIX_W        (std::uint8_t)0b00001000
#define REX_PREFIX_R        (std::uint8_t)0b00000100
#define REX_PREFIX_X        (std::uint8_t)0b00000010
#define REX_PREFIX_B        (std::uint8_t)0b00000001


#define OPCODE_TWO_BYTE_PREFIX              (std::uint8_t)0x0f
#define OPCODE_SYSCALL                      (std::uint8_t)0x05

struct UnknownRegisterIndex
{

};

class X86_64Writer : public AssemblyWriterInterface
{

public:

    void
    writeRoutine(Routine *routine);

private:
    void
    writeMoveImmediateOpcode(unsigned int registerIndex);

    void
    writeMoveInt32ToRegisterOpcode(unsigned int registerIndex, Int32* integer);

    void
    writeMoveInt64ToRegisterOpcode(unsigned int registerIndex, Int64* integer);

    void
    addInt32(Int32* integer);

    void
    addInt64(Int64* integer);

    List<std::uint8_t>*
    _routineOutput;

    List<std::uint32_t>
    _dataRelocations;
};


}


#endif //ELET_X86_64WRITER_H
