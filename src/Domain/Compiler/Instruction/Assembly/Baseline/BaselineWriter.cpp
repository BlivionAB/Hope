#include "BaselineWriter.h"
#include <Foundation/Utf8String.h>
#include <Foundation/Stack.h>
#include <variant>


namespace elet::domain::compiler::instruction::output
{


BaselineWriter::BaselineWriter():
    AssemblyWriterInterface(),
    _tw(new TextWriter())
{

}


void
BaselineWriter::writeStartRoutine(FunctionRoutine* routine, std::size_t offset)
{
    writeFunctionRoutine(routine);
    std::cout << "Begin:" << std::endl;
    std::cout << _tw->toString();
    std::cout << "End:" << std::endl;
}

void
BaselineWriter::writeFunctionRoutine(FunctionRoutine* routine)
{
    _tw->clearIndent();
    _tw->write(routine->name);
    _tw->writeLine(":");
    _tw->indent();
    unsigned int parameterIndex = 0;
    for (const auto& parameter : routine->parameters)
    {
        _tw->write("Par");
        _tw->writeUnsignedInteger(parameter->size);
        _tw->space();
        _tw->write("P");
        _tw->writeUnsignedInteger(parameterIndex);
        _tw->writeLine(";");
    }
    for (const auto& instruction : routine->instructions)
    {
        switch (instruction->kind)
        {
            case InstructionKind::Call:
                writeCallInstruction(reinterpret_cast<CallInstruction*>(instruction));
                break;
        }
    }
}



void
BaselineWriter::writeArgumentDeclaration(ArgumentDeclaration* argumentDeclaration, unsigned int argumentIndex)
{
    _tw->write("Arg");
    _tw->writeUnsignedInteger(argumentDeclaration->size);
    _tw->space();
    _tw->write("A");
    _tw->writeUnsignedInteger(argumentIndex);
    _tw->space();
    if (auto string = std::get_if<output::CString*>(&argumentDeclaration->value))
    {
        _tw->write("\"");
        _tw->write((*string)->value);
        _tw->write("\"");
    }
    else if (auto variable = std::get_if<output::ParameterDeclaration*>(&argumentDeclaration->value))
    {
        _tw->write("P");
        _tw->writeUnsignedInteger((*variable)->index);
    }
    _tw->writeLine(";");
}


void
BaselineWriter::writeCallInstruction(const CallInstruction* callInstruction)
{
    unsigned int i = 0;
    for (const auto& arg : callInstruction->arguments)
    {
        writeArgumentDeclaration(arg, i);
        ++i;
    }
    _tw->write("Call ");
    if (callInstruction->routine->kind == RoutineKind::Function)
    {
        _tw->write(reinterpret_cast<FunctionRoutine*>(callInstruction->routine)->name);
    }
    else if (callInstruction->routine->kind == RoutineKind::External)
    {
        _tw->write(reinterpret_cast<ExternalRoutine*>(callInstruction->routine)->name);
    }
    _tw->write(";");
    _tw->unindent();
    _tw->newline();
    auto routine = callInstruction->routine;
    if (routine->kind == RoutineKind::Function)
    {
        writeFunctionRoutine(reinterpret_cast<FunctionRoutine*>(routine));
    }
}


void
BaselineWriter::writeOperand(output::Operand *operand)
{
    _tw->space();
    switch (operand->kind)
    {
        case OperandKind::Label:
            writeLabel(reinterpret_cast<output::Label*>(operand));
            break;
        case OperandKind::Register:
            writeRegister(reinterpret_cast<output::Register*>(operand));
            break;
        case OperandKind::Int32:
            writeInteger(reinterpret_cast<output::Int32*>(operand));
            break;
        case OperandKind::VariableReference:
            writeVariableReference(reinterpret_cast<output::VariableReference *>(operand));
            break;
        default:
            throw UnknownRegisterError();
    }
}


void
BaselineWriter::writeRegister(output::Register *_register)
{
    _tw->writeCharacter('R');
    _tw->writeCharacter(static_cast<char>(_register->index) + static_cast<char>(Utf8String::Character::_0));
}


void
BaselineWriter::writeInteger(output::Int32* value)
{
    _tw->write("0x");
    unsigned int v = value->value;
    unsigned int remainder;
    Stack<char> characterStack;
    do {
        remainder = v % 16;
        v = v / 16;
        if (remainder <= 9)
        {
            characterStack.push(remainder + static_cast<int>(Utf8String::Character::_0));
        }
        else
        {
            characterStack.push(remainder + static_cast<int>(Utf8String::Character::W));
        }
    }
    while (v != 0);
    char character;
    std::size_t size = characterStack.size();
    for (int i = 0; i < size; i++)
    {
        character = characterStack.top();
        characterStack.pop();
        _tw->writeCharacter(character);
    }
    _tw->write(" /* ");
    v = value->value;
    do
    {
        remainder = v % 10;
        v = v / 10;
        characterStack.push(remainder + static_cast<int>(Utf8String::Character::_0));
    }
    while (v != 0);
    size = characterStack.size();
    for (int i = 0; i < size; i++)
    {
        character = characterStack.top();
        characterStack.pop();
        _tw->writeCharacter(character);
    }
    _tw->write(" */");
}


void
BaselineWriter::writeLabel(output::Label* label)
{
    _tw->write(label->symbol);
}

void
BaselineWriter::writeVariableReference(output::VariableReference* variableReference)
{
    if (variableReference->isParameter)
    {
        _tw->write("Param");
    }
    else
    {
        _tw->write("Var");
    }
    _tw->writeSize(variableReference->index);
}


}