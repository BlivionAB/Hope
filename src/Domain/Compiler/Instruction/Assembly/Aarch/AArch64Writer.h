#ifndef ELET_AARCH64WRITER_H
#define ELET_AARCH64WRITER_H

#include "../AssemblyWriterInterface.h"


namespace elet::domain::compiler::instruction::output
{

class AArch64Writer : public AssemblyWriterInterface
{

public:

    AArch64Writer(List<uint8_t>* output);

    void
    writeTextSection(FunctionRoutine* routine) override;

    void
    writeStubs() override;

    void
    writeStubHelper() override;

private:

    void
    writeFunction(FunctionRoutine* routine);

    void
    writeFunctionPrologue(FunctionRoutine* routine, uint64_t& stackSize);

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
    simm26(int32_t value) const;

    void
    writeFunctionEpilogue(FunctionRoutine* routine, uint64_t& stackSize, uint64_t& stackOffset);

    uint64_t
    writeFunctionInstructions(FunctionRoutine* routine);

    uint64_t
    writeCallInstruction(CallInstruction* instruction, FunctionRoutine* routine);

    void
    writeFunctionRelocationAddresses(FunctionRoutine* routine);
};

}

#endif //ELET_AARCH64WRITER_H
