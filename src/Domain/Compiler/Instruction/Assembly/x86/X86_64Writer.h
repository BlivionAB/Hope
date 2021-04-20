#ifndef ELET_X86_64WRITER_H
#define ELET_X86_64WRITER_H

#include <Domain/Compiler/Instruction/Instruction.h>

#include <queue>

#include <Domain/Compiler/Instruction/Assembly/AssemblyWriterInterface.h>
#include <Domain/Compiler/Instruction/Instruction.h>
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/Mov.h"
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/GeneralOpCodes.h"
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/AddressForm32.h"

namespace elet::domain::compiler::instruction::output
{


struct Int32;
struct Int64;

struct StringReference;

#define DISP8_MAX_VALUE     0x100
#define DISP16_MAX_VALUE    0x10000
#define DISP32_MAX_VALUE    0x100000000

#define REX_PREFIX_MAGIC    (std::uint8_t)0b01000000
#define REX_PREFIX_W        (std::uint8_t)0b00001000
#define REX_PREFIX_R        (std::uint8_t)0b00000100
#define REX_PREFIX_X        (std::uint8_t)0b00000010
#define REX_PREFIX_B        (std::uint8_t)0b00000001

struct UnknownRegisterIndex
{

};

struct ModRmByte
{
    std::uint8_t
    rm: 3,
    reg: 3,
    mod: 2;
};

class X86_64Writer : public AssemblyWriterInterface
{

public:

    X86_64Writer();

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
    writeUint32(std::uint32_t integer);

    void
    writeUint64(std::uint64_t integer);

    void
    writeLoadEffectiveAddressFromStringReference(unsigned char registerIndex, StringReference* stringReference);

    List<Int64*>*
    _relocations;

    Routine*
    _currentRoutine;

    List<std::uint8_t>*
    _routineOutput;
};


}


#endif //ELET_X86_64WRITER_H
