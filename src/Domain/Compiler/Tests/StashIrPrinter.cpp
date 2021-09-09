#include <Domain/Compiler/Instruction/Instruction.h>
#include "StashIrPrinter.h"

namespace elet::domain::compiler::test
{


Utf8String
StashIRPrinter::writeFunctionRoutines(std::queue<output::FunctionRoutine*>& instructions)
{
    while (!instructions.empty())
    {
        output::FunctionRoutine* routine = instructions.front();
        writeFunctionRoutine(routine);
        instructions.pop();
    }
    return _tw.toString();
}


void
StashIRPrinter::writeFunctionRoutine(output::FunctionRoutine* functionRoutine)
{
    _tw.write(functionRoutine->name);
    _tw.write("(");
    for (const output::ParameterDeclaration* parameterDeclaration : functionRoutine->parameters)
    {
        _tw.write(parameterDeclaration->size);
    }
    _tw.write("):");
    _tw.newline();
    _tw.indent();
    for (const output::Instruction* instruction : functionRoutine->instructions)
    {
        writeInstruction(instruction);
    }
    _tw.unindent();
}


void
StashIRPrinter::writeInstruction(const output::Instruction* instruction)
{
    switch (instruction->kind)
    {
        case output::InstructionKind::VariableDeclaration:
            writeVariableDeclaration(reinterpret_cast<const output::VariableDeclaration*>(instruction));
            break;
        default:
            throw std::runtime_error("Unknown instruction");
    }
}


void
StashIRPrinter::writeVariableDeclaration(const output::VariableDeclaration* variableDeclaration)
{
    if (std::holds_alternative<output::ImmediateValue>(variableDeclaration->expression))
    {
        _tw.write("Store ");
        _tw.write(std::get<int64_t>(std::get<output::ImmediateValue>(variableDeclaration->expression)));
        _tw.newline();
    }
    else // std::holds_alternative<output::ScratchRegister*>
    {
        const output::ScratchRegister* scratchRegister = std::get<output::ScratchRegister*>(variableDeclaration->expression);
        writeOperation(scratchRegister->operation);
    }
}

void
StashIRPrinter::writeOperation(output::Operation* operation)
{
    switch (operation->kind)
    {
        case output::OperationKind::ImmediateToMemory:
            _tw.write("Store");
            break;
    }
}


}
