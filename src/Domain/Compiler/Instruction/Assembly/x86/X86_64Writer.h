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
struct CallInstruction;

struct CallingConvention
{
    std::vector<std::uint8_t>
    registers;
};


class X86_64Writer : public AssemblyWriterInterface
{

public:

    X86_64Writer(List<std::uint8_t>* output);

    void
    writeTextSection(FunctionRoutine* routine);


    void
    writeStubs();

    void
    writeStubHelper();

private:

    CallingConvention
    _callingConvention = { { Reg7, Reg6, Reg2, Reg1 } };

    size_t
    _subtractStackAddress;

    void
    writeFunction(FunctionRoutine* routine);

    uint64_t
    writeFunctionPrologue(size_t stackSize);

    void
    writeFunctionEpilogue(size_t stackSize, uint64_t routineSize);

    uint64_t
    writeParameter(uint64_t size, unsigned int index, uint64_t& stackOffset);

    uint64_t
    writeFunctionParameters(const FunctionRoutine* routine, uint64_t& stackOffset);

    uint64_t
    getStackSizeFromFunctionParameters(const FunctionRoutine* routine);

    uint64_t
    writeFunctionInstructions(FunctionRoutine* routine);

    uint64_t
    writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine);

    uint64_t
    writeCallInstructionArguments(CallInstruction* callInstruction);

    void
    writeMoveFromOffset(uint8_t reg, size_t offset);

    void
    writePointer(std::uint64_t address);

    void
    writeFunctionRelocationAddresses(FunctionRoutine* routine);

    void
    writeInstructionsPadding(uint64_t length);
};


}


#endif //ELET_X86_64WRITER_H
