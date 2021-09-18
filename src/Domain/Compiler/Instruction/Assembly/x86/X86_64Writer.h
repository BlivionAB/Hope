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

private:

    size_t
    _subtractStackAddress;

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
    writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* function) override;

    uint64_t
    writeCallInstructionArguments(CallInstruction* callInstruction);

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
    writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function) override;

    void
    writePushInstruction(PushInstruction* pushInstruction, FunctionRoutine* function) override;

    void
    writePopInstruction(PopInstruction* popInstruction, FunctionRoutine* function) override;

    void
    writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function) override;

    void
    writeMoveRegisterInstruction(MoveRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function) override;

    void
    writeMoveAddressInstruction(MoveAddressInstruction* moveAddressInstruction, FunctionRoutine* function) override;

    void
    writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function) override;

    void
    writeAddRegisterInstruction(AddRegisterInstruction* addRegisterInstruction, FunctionRoutine* function) override;

    void
    writeAddImmediateInstruction(AddImmediateInstruction* addImmediateInstruction, FunctionRoutine* function) override;

    void
    writeSubtractImmediateInstruction(SubtractImmediateInstruction* subtractImmediateInstruction, FunctionRoutine* function) override;

    void
    writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function) override;

    void
    writeResetRegisterInstruction(ResetRegisterInstruction* resetResetRegisterInstruction, FunctionRoutine* function) override;

    virtual
    void
    writeInstructionsPadding(FunctionRoutine* function) override;

    RegisterBits
    getRegisterBitsFromOperandRegister(OperandRegister operandRegister);

    RmBits
    getRmBitsFromOperandRegister(OperandRegister operandRegister);

    void
    writeModRmAndStackOffset(LoadInstruction* loadInstruction, FunctionRoutine* function);

    void
    writeEbpReferenceBytes(uint64_t stackOffset, OperandRegister operandRegister, FunctionRoutine* function);
};


}


#endif //ELET_X86_64WRITER_H
