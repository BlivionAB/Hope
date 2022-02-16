#ifndef ELET_STASHIRPRINTER_H
#define ELET_STASHIRPRINTER_H


#include <queue>
#include <Domain/Compiler/Instruction/Instruction.h>
#include <Foundation/TextWriter.h>


using namespace elet::domain::compiler::instruction;


namespace elet::domain::compiler::test
{
    class StashIRPrinter
    {
    public:

        Utf8String
        writeFunctionRoutines(std::queue<output::FunctionRoutine*>& instructions);

        void
        writeFunctionRoutine(output::FunctionRoutine* function);

    private:

        TextWriter
        _tw;

        void
        writeInstruction(const output::Instruction* instruction, output::FunctionRoutine* function);

        void
        writeStoreImmediateInstruction(const output::StoreImmediateInstruction* instruction);

        void
        writeLoadUnsignedInstruction(const output::LoadUnsignedInstruction* instruction);

        void
        writeOperandRegister(output::OperandRegister operandRegister);

        void
        writeMoveRegisterInstruction(const output::MoveRegisterToRegisterInstruction* instruction);

        void
        writeMoveAddressInstruction(const output::MoveAddressToRegisterInstruction* moveAddressInstruction);

        void
        writeCallInstruction(const output::CallInstruction* callInstruction, output::FunctionRoutine* function);

        void
        writeStoreRegisterInstruction(const output::StoreRegisterInstruction* storeRegisterInstruction);

        void
        writeMoveImmediateInstruction(const output::MoveImmediateInstruction* moveImmediateInstruction);

        void
        writeRegisterToRegisterOperation(Utf8String operationName, const output::OperationRegisterToRegisterInstruction* instruction);

        void
        writeOperationRegisters(const output::OperationRegisterToRegisterInstruction* operationInstruction);

        void
        writeImmediateToRegisterOperation(const char* operationName, const output::OperationImmediateToRegisterInstruction* instruction);

        void
        writeOperationName(Utf8String name, const output::Instruction* instruction);

        void
        writeOperationName(Utf8String name, RegisterSize registerSize);

        void
        writeMoveZeroExtendInstruction(const output::MoveZeroExtendInstruction* instruction);

        void
        writeMoveSignExtend(const output::MoveSignExtendInstruction* instruction);

        void
        writeLoadSignedInstruction(const output::LoadSignedInstruction* instruction);
    };
}
#endif //ELET_STASHIRPRINTER_H
