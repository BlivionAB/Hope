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
        case output::InstructionKind::Load:
            writeLoadInstruction(reinterpret_cast<const output::LoadInstruction*>(instruction));
            break;
        case output::InstructionKind::MoveRegister:
            writeMoveRegisterInstruction(reinterpret_cast<const output::MoveRegisterInstruction*>(instruction));
            break;
        case output::InstructionKind::Return:
            _tw.write("Ret");
            break;
        case output::InstructionKind::AddRegisterToRegister:
            _tw.write("Add OpL, OpL, OpR");
            break;
        default:
            throw std::runtime_error("Unknown instruction");
    }
    _tw.newline();
}


void
StashIRPrinter::writeMoveRegisterInstruction(const output::MoveRegisterInstruction* moveRegisterInstruction)
{
    _tw.write("Mov ");
    writeOperandRegister(moveRegisterInstruction->destination);
    _tw.write(", ");
    writeOperandRegister(moveRegisterInstruction->target);
}


void
StashIRPrinter::writeLoadInstruction(const output::LoadInstruction* loadInstruction)
{
    _tw.write("Ldr ");
    writeOperandRegister(loadInstruction->destination);
    _tw.write(", [Sp + ");
    _tw.write(loadInstruction->stackOffset);
    _tw.write("]");
}


void
StashIRPrinter::writeOperandRegister(output::OperandRegister operandRegister)
{
    switch (operandRegister)
    {
        case output::OperandRegister::Return:
            _tw.write("OpRet");
            break;
        case output::OperandRegister::Left:
            _tw.write("OpL");
            break;
        case output::OperandRegister::Right:
            _tw.write("OpR");
            break;
    }
}


void
StashIRPrinter::writeVariableDeclaration(const output::VariableDeclaration* variableDeclaration)
{
    if (std::holds_alternative<output::ImmediateValue>(variableDeclaration->expression))
    {
        _tw.write("Str [Sp + ");
        _tw.write(variableDeclaration->stackOffset);
        _tw.write("], ");
        output::ImmediateValue value = std::get<output::ImmediateValue>(variableDeclaration->expression);
        if (std::holds_alternative<int32_t>(value))
        {
            _tw.write(std::get<int32_t>(value));
        }
        else
        {
            _tw.write(std::get<int64_t>(value));
        }
    }
    else // std::holds_alternative<output::ScratchRegister*>
    {
        _tw.write("Str [Sp + ");
        _tw.write(variableDeclaration->stackOffset);
        _tw.write("], ");
        _tw.write("Op1");

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
