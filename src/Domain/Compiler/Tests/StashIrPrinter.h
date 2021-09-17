#ifndef ELET_STASHIRPRINTER_H
#define ELET_STASHIRPRINTER_H


#include <queue>
#include <Domain/Compiler/Instruction/Instruction.h>
#include <Domain/Compiler/Instruction/TextWriter.h>


using namespace elet::domain::compiler::instruction;


namespace elet::domain::compiler::test
{


class StashIRPrinter
{

public:

    Utf8String
    writeFunctionRoutines(std::queue<output::FunctionRoutine*>& instructions);

    void
    writeFunctionRoutine(output::FunctionRoutine* functionRoutine);

private:

    TextWriter
    _tw;

    void
    writeInstruction(const output::Instruction* instruction);

    void
    writeStoreImmediateInstruction(const output::StoreImmediateInstruction* storeImmediateInstruction);

    void
    writeOperation(output::Operation* operation);

    void
    writeLoadInstruction(const output::LoadInstruction* loadInstruction);

    void
    writeOperandRegister(output::OperandRegister operandRegister);

    void
    writeMoveRegisterInstruction(const output::MoveRegisterInstruction* moveRegisterInstruction);
};


}
#endif //ELET_STASHIRPRINTER_H
