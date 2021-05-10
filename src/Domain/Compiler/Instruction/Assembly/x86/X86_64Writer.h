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
struct CallInstruction;

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


struct CallingConvention
{
    std::vector<std::uint8_t>
    registers;
};


class X86_64Writer : public AssemblyWriterInterface
{

public:

    X86_64Writer();

    void
    writeStartRoutine(FunctionRoutine* routine, std::size_t offset);

private:

    std::size_t
    _currentOffset;

    std::size_t
    _localStackOffset;

    List<Int64*>*
    _relocations;

    List<std::uint8_t>*
    _routineOutput;

    CallingConvention
    _callingConvention = { { REG_EDI, REG_ESI, REG_EDX, REG_ECX } };

    void
    writeFunctionRoutine(FunctionRoutine* pRoutine);

    void
    writeFunctionPrelude();

    void
    writeByte(std::uint8_t instruction);

    void
    writeDoubleWord(std::uint32_t instruction);

    void
    writeQuadWord(std::uint64_t instruction);

    void
    writeFunctionPostlude();

    void
    writeParameter(size_t size, unsigned int index);

    void
    writeFunctionParameters(const FunctionRoutine* routine);

    void
    writeFunctionInstructions(const FunctionRoutine* routine);

    void
    writeCallInstruction(CallInstruction* callInstruction);

    void
    writeCallInstructionArguments(CallInstruction* callInstruction);

    void
    writeMoveFromOffset(uint8_t reg, size_t offset);

    void
    writePointer(std::uint64_t address);
};


}


#endif //ELET_X86_64WRITER_H
