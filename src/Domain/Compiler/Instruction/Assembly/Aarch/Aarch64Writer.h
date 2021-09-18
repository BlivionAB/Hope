#ifndef ELET_AARCH64WRITER_H
#define ELET_AARCH64WRITER_H

#include "../AssemblyWriterInterface.h"
#include "AArch64Encodings.h"


namespace elet::domain::compiler::instruction::output
{

class Aarch64Writer final : public AssemblyWriterInterface
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

    void
    relocateStubHelperOffset(uint64_t offset, uint64_t stubHelperAddress, uint64_t textSegmentStartOffset) override;

    void
    relocateGotBoundRoutine(uint64_t gotOffset, uint64_t offset);

private:

//    void
//    writeFunctionPrologue(FunctionRoutine* function) override;
//
//    void
//    writeFunctionEpilogue(FunctionRoutine* routine) override;
//
//    void
//    writeStartFunctionEpilogue(FunctionRoutine* function) override;

//    void
//    writeParameter(ParameterDeclaration* parameterDeclaration, unsigned int index, FunctionRoutine* routine) override;

    void
    writeFunctionRelocationAddresses(FunctionRoutine* routine);

    void
    writeAddImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value);

    void
    writeSubImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value);

    void
    writeBr(Aarch64Register rn);

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

    void
    writeB(int32_t offset);

    void
    writeInstructionsPadding(uint64_t i);

    void
    writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function) override;

    void
    writePushInstruction(PushInstruction* pushInstruction, FunctionRoutine* function) override;

    void
    writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function) override;

    void
    writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* function) override;
};

}

#endif //ELET_AARCH64WRITER_H