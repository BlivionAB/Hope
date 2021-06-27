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
    writeFunctionPrologue(FunctionRoutine* routine);

    constexpr
    uint32_t
    Rd(uint32_t reg) const;

    constexpr
    uint32_t
    Rt(uint32_t reg) const;

    constexpr
    uint32_t
    Rn(uint32_t reg) const;

    constexpr
    uint32_t
    Rt2(uint32_t reg) const;

    constexpr
    uint32_t
    imm12(int16_t value) const;

    constexpr
    uint32_t
    imm7(int8_t value) const;
};

}

#endif //ELET_AARCH64WRITER_H
