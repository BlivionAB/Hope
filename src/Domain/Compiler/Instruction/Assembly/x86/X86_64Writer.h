#ifndef ELET_X86_64WRITER_H
#define ELET_X86_64WRITER_H

#include <Domain/Compiler/Instruction/Instruction.h>

#include <queue>

#include <Domain/Compiler/Instruction/Assembly/AssemblyWriterInterface.h>
#include <Domain/Compiler/Instruction/Instruction.h>
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/GeneralOpCodes.h"
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/AddressForm32.h"

namespace elet::domain::compiler::instruction::output
{


struct Int32;
struct CallInstruction;


class X86_64Writer : public AssemblyWriterInterface
{

public:

    X86_64Writer(List<std::uint8_t>* output);

    void
    writeTextSection(FunctionRoutine* routine) override;

    void
    writeStubs() override;

    void
    writeStubHelper() override;

    void
    writeCStringSection() override;

private:

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
    writeFunctionInstructions(FunctionRoutine* routine, uint64_t& stackOffset);

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

    void
    relocateStub(uint64_t offset, uint64_t textSegmentStartOffset, ExternalRoutine* externalRoutine) override;

    void
    relocateDyldPrivate(uint64_t dataSectionOffset, uint64_t textSegmentStartOffset) override;

    void
    relocateStubHelperOffset(uint64_t offset, uint64_t stubHelperAddress, uint64_t textSegmentStartOffset) override;

    void
    relocateGotBoundRoutine(uint64_t gotOffset, uint64_t offset);

    void
    writeVariableDeclaration(VariableDeclaration* variableDeclaration, uint64_t& size, uint64_t& stackOffset);
};


}


#endif //ELET_X86_64WRITER_H
