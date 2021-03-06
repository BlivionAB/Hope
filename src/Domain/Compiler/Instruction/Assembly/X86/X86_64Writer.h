#ifndef ELET_X86_64WRITER_H
#define ELET_X86_64WRITER_H

#include <Domain/Compiler/Instruction/Instruction.h>

#include <queue>

#include <Domain/Compiler/Instruction/Assembly/AssemblyWriterInterface.h>
#include <Domain/Compiler/Instruction/Instruction.h>
#include "Domain/Compiler/Instruction/Assembly/X86/OpCode/GeneralOpCodes.h"
#include "Domain/Compiler/Instruction/Assembly/X86/OpCode/AddressForm32.h"

namespace elet::domain::compiler::instruction::output::x86
{
    struct Int32;
    struct CallInstruction;


    enum class Register
    {
        Rax,
        Rcx,
        Rdx,
        Rbx,
        Rsp,
        Rbp,
        Rsi,
        Rdi,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15,
    };


    class X86_64Writer final : public AssemblyWriterInterface
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

        void
        setCallingConvention(CallingConvention callingConvention) override;

    private:

        size_t
        _subtractStackAddress;

        List<RegBits>
        _parameterRegisters;

    //    void
    //    writeFunction(FunctionRoutine* routine);

    //    void
    //    writeFunctionPrologue(FunctionRoutine* functionRoutine) override;
    //
    //    void
    //    writeFunctionEpilogue(FunctionRoutine* function) override;
    //
    //    void
    //    writeStartFunctionEpilogue(FunctionRoutine* function) override;

    //    void
    //    writeParameter(ParameterDeclaration* parameterDeclaration, unsigned int index, FunctionRoutine* routine) override;

        void
        writeCallInstruction(output::CallInstruction* callInstruction, FunctionRoutine* function) override;

        uint64_t
        writeCallInstructionArguments(output::CallInstruction* callInstruction);

        void
        writePointer(std::uint64_t address);

        void
        writeFunctionRelocationAddresses(FunctionRoutine* routine) override;

        void
        writeInstructionsPadding(uint64_t length);

        void
        relocateStub(uint64_t offset, uint64_t textSegmentStartOffset, ExternalRoutine* externalRoutine) override;

        void
        relocateDyldPrivate(uint64_t dataSectionOffset, uint64_t textSegmentStartOffset) override;

        void
        relocateStubHelperOffset(uint64_t offset, uint64_t stubHelperAddress, uint64_t textSegmentStartOffset) override;

        void
        relocateGotBoundRoutine(uint64_t gotOffset, uint64_t offset) override;

        void
        writeStoreImmediateInstruction(StoreImmediateInstruction* instruction, FunctionRoutine* function) override;

        void
        writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function) override;

        void
        writeMoveImmediateInstruction(MoveImmediateInstruction* moveImmediateInstruction, FunctionRoutine* function) override;

        void
        writeMoveRegisterToRegisterInstruction(MoveRegisterToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeMoveZeroExtendInstruction(MoveZeroExtendInstruction* instruction, FunctionRoutine* function) override;

        void
        writeMoveSignExtendInstruction(MoveSignExtendInstruction* instruction, FunctionRoutine* function) override;

        void
        writeMoveAddressInstruction(MoveAddressToRegisterInstruction* moveAddressInstruction, FunctionRoutine* function) override;

        void
        writeLoadUnsignedInstruction(LoadUnsignedInstruction* instruction, FunctionRoutine* function) override;

        void
        writeLoadSignedInstruction(LoadSignedInstruction* instruction, FunctionRoutine* function) override;

        void
        writeAddRegisterToRegisterInstruction(AddRegisterToRegisterInstruction* addRegisterInstruction, FunctionRoutine* function) override;

        void
        writeSubtractRegisterToRegisterInstruction(SubtractRegisterToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeAddImmediateInstruction(AddImmediateToRegisterInstruction* addImmediateInstruction, FunctionRoutine* function) override;

        void
        writeAddAddressToRegisterInstruction(AddAddressToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeAddImmediateInstruction(OperandRegister destination, uint64_t value, FunctionRoutine* function);

        void
        writeSubtractImmediateInstruction(SubtractImmediateToRegisterInstruction* subtractImmediateInstruction, FunctionRoutine* function) override;

        void
        writeSubtractImmediateInstruction(OperandRegister destination, uint64_t value, FunctionRoutine* function);

        void
        writeSubtractRegisterAddressInstruction(SubtractRegisterToAddressInstruction* instruction, FunctionRoutine* function) override;

        void
        writeMultiplySignedRegisterAddressInstruction(MultiplySignedAddressToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeMultiplySignedRegisterToRegisterInstruction(MultiplySignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeDivideUnsignedRegisterAddressInstruction(DivideUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeDivideSignedRegisterAddressInstruction(DivideSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeDivideSignedRegisterToRegisterInstruction(DivideSignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeModuloUnsignedRegisterAddressInstruction(ModuloUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeModuloSignedRegisterAddressInstruction(ModuloSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function) override;

        void
        writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function) override;

        void
        writeFunctionPrologue(FunctionRoutine* function) override;

        void
        writeFunctionEpilogue(FunctionRoutine* function) override;

        void
        writeInstructionsPadding(FunctionRoutine* function) override;

        Register
        getRegisterFromOperandRegister(OperandRegister operandRegister);

        RmBits
        getRmBitsFromOperandRegister(OperandRegister operandRegister);

        RegBits
        getRegBitsFromOperandRegister(OperandRegister operandRegister);

        void
        writeModRmAndStackOffset(LoadUnsignedInstruction* loadInstruction, FunctionRoutine* function);

        void
        writeEbpReferenceBytes(uint64_t stackOffset, OperandRegister operandRegister, FunctionRoutine* function);

        void
        writeEbpReferenceBytes(uint64_t stackOffset, uint8_t registerBits, FunctionRoutine* function);

        void
        writeEspReferenceBytes(uint64_t stackOffset, OperandRegister operandRegister, FunctionRoutine* function);

        void
        writeSysVPrologue(FunctionRoutine* function);

        void
        writeMicrosoftX64Prologue(FunctionRoutine* function);

        void
        writeSysVFunctionEpilogue(FunctionRoutine* function);

        void
        writeMicrosoftX64Epilogue(FunctionRoutine* function);

        uint8_t
        getMicrosoftX64StackAllocationSize(const FunctionRoutine* function) const;

        uint8_t
        getRegBitsFromRegister(Register _register);

        void
        writeQuadInstructionMultipleInFunction(std::initializer_list<uint8_t> encodingMultiple, Instruction* instruction, FunctionRoutine* function);

        void
        writeQuadInstructionInFunction(uint8_t encoding, Instruction* instruction, FunctionRoutine* function);

        void
        writeStackReferenceBytes(StackOffsetInstruction* instruction, FunctionRoutine* function);
    };
}


#endif //ELET_X86_64WRITER_H
