#ifndef ELET_AARCH64WRITER_H
#define ELET_AARCH64WRITER_H

#include "../AssemblyWriterInterface.h"
#include "AArch64Encodings.h"


namespace elet::domain::compiler::instruction::output
{
    enum class Shift : uint32_t
    {
        LSL = 0,
        LSR = 1,
        ASR = 2,
    };

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

        void
        writeFunctionPrologue(FunctionRoutine* function) override;

        void
        writeFunctionEpilogue(FunctionRoutine* function) override;

        void
        writeMoveImmediateInstruction(MoveImmediateInstruction* moveImmediateInstruction, FunctionRoutine* function) override;

        void
        writeMoveAddressInstruction(MoveAddressInstruction* moveAddressInstruction, FunctionRoutine* function) override;

        void
        writeMoveRegisterInstruction(MoveRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function) override;

        void
        writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function) override;

        void
        writeAddRegisterInstruction(AddRegisterToRegisterInstruction* addRegisterInstruction, FunctionRoutine* function) override;

        void
        writeInstructionInFunction(uint32_t instruction, Instruction* referenceInstruction, FunctionRoutine* function);
    private:

        List<Aarch64Register>
        _parameterRegisters;

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
        writeAddImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value, output::FunctionRoutine* function);

        void
        writeSubImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value, output::FunctionRoutine* function);

        void
        writeInstructionsPadding(FunctionRoutine* function) override;

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

        uint32_t
        hw(uint32_t leftShift) const;

        void
        writeInstructionsPadding(uint64_t i);

        void
        writeStoreImmediateInstruction(StoreImmediateInstruction* instruction, FunctionRoutine* function) override;

        void
        writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function) override;

        void
        writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* function) override;

        int
        getAarch64RegisterFromOperandRegister(OperandRegister operandRegister) const;

        bool
        moveWideIsPreferred(uint8_t sf, uint8_t N, uint8_t imms, uint8_t immr) const;

        bool
        processLogicalImmediate(uint64_t imm, const Aarch64Register& rd, RegisterBitSize registerSize,
                                output::FunctionRoutine* function);

        void
        writeInstruction(uint32_t instruction, uint64_t value, FunctionRoutine* function);

        void
        writeNegatedOrRegularShiftMoves(uint64_t value, const Aarch64Register& rd, RegisterBitSize registerSize, FunctionRoutine* function);

        bool
        processNegatedImmediateEncoding(uint64_t value, const Aarch64Register& rd, const RegisterBitSize& registerSize,
                                        output::FunctionRoutine* function) const;

        void
        processPositiveValues(uint64_t value, const Aarch64Register& rd, const RegisterBitSize& registerSize,
                              FunctionRoutine* function);

        uint32_t
        shift(Shift shift);
    };
}

#endif //ELET_AARCH64WRITER_H
