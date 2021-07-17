#ifndef ELET_AARCH64WRITER_H
#define ELET_AARCH64WRITER_H

#include "../AssemblyWriterInterface.h"
#include "AArch64Encodings.h"


namespace elet::domain::compiler::instruction::output
{

class Aarch64Writer : public AssemblyWriterInterface
{

public:

    Aarch64Writer(List<uint8_t>* output);

    void
    writeTextSection(FunctionRoutine* routine) override;

    void
    writeStubs() override;

    void
    writeStubHelper() override;

    void
    writeCStringSection() override;

    void
    relocateCStrings(uint64_t textSegmentStartOffset) override;

    void
    relocateStub(uint64_t offset, uint64_t textSegmentStartOffset, ExternalRoutine* externalRoutine) override;

    void
    relocateDyldPrivate(uint64_t dataSectionOffset, uint64_t textSegmentStartOffset) override;

private:

    void
    writeFunction(FunctionRoutine* routine);

    void
    writeFunctionPrologue(FunctionRoutine* routine, uint64_t& stackOffset, uint64_t& stackSize,
                          uint64_t& routineSize);

    void
    writeFunctionParameters(const FunctionRoutine* routine, uint64_t& stackOffset, uint64_t& stackSize,
                            uint64_t& routineSize);

    void
    writeParameter(uint64_t size, unsigned int index, uint64_t& stackOffset, uint64_t& stackSize,
                   uint64_t& routineSize);

    void
    writeFunctionEpilogue(FunctionRoutine* routine, uint64_t& stackSize, uint64_t& routineSize);

    void
    writeFunctionInstructions(FunctionRoutine* routine, uint64_t& routineSize);

    uint64_t
    writeCallInstruction(CallInstruction* instruction, FunctionRoutine* routine);

    void
    writeFunctionRelocationAddresses(FunctionRoutine* routine);

    void
    writeAddImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value);

    void
    writeSubImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value);

    uint32_t
    Rd(uint32_t reg) const;

    uint32_t
    Rt(uint32_t reg) const;

    uint32_t
    Rn(uint32_t reg) const;

    uint32_t
    Rt2(uint32_t reg) const;

    uint32_t
    Rm(uint8_t reg) const;

    uint32_t
    uimm6(uint8_t value) const;

    uint32_t
    uimm12(uint16_t value) const;

    uint32_t
    uimm16(uint16_t value) const;

    uint32_t
    simm7(int8_t value) const;

    uint32_t
    simm19(int32_t value) const;

    uint32_t
    simm26(int32_t value) const;

    uint32_t
    immhilo(int32_t value) const;

    uint64_t
    writeCallInstructionArguments(CallInstruction* callInstruction, FunctionRoutine* parentRoutine);
};

}

#endif //ELET_AARCH64WRITER_H
