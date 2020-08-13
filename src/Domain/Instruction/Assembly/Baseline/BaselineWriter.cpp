#include "BaselineWriter.h"
#include <Foundation/Utf8String.h>
#include <Foundation/Stack.h>


namespace elet::domain::compiler::instruction::output
{

thread_local
TextWriter*
BaselineWriter::_tw = nullptr;


BaselineWriter::BaselineWriter():
    AssemblyWriterInterface()
{

}


BaselineWriter::~BaselineWriter()
{
    for (auto tw : _textWriters)
    {
        delete tw;
    }
}


void
BaselineWriter::writeRoutine(Routine *routine)
{
    _tw = createTextWriter();
    if (routine->kind == RoutineKind::Routine)
    {
        _tw->write(routine->name);
        _tw->writeCharacter(':');
        _tw->newline();
        _tw->indent();
        for (const auto& instruction : *routine->instructions)
        {
            writeInstruction(instruction);
        }
        _tw->unindent();
    }
    else if (routine->kind == RoutineKind::Function)
    {
        Function* _function = reinterpret_cast<Function*>(routine);
        _tw->write(_function->name);
        _tw->writeCharacter('(');
        bool firstParameter = true;
        for (const Parameter* parameter : _function->parameters)
        {
            if (!firstParameter)
            {
                _tw->write(", ");
            }
            _tw->write(parameter->typeLabel);
            firstParameter = false;
        }
        _tw->writeCharacter(')');
        _tw->writeCharacter(':');
        _tw->indent();
        _tw->newline();
        for (output::Instruction* instruction : *routine->instructions)
        {
            writeInstruction(instruction);
        }
        _tw->unindent();
    }
    else
    {
        throw UnknownRoutineError();
    }
    _tw->newline();
    auto result = new List<std::uint8_t>();
    auto string = _tw->toString();
    for (const auto& ch : string)
    {
        result->add(static_cast<std::uint8_t>(ch));
    }
}


void
BaselineWriter::writeInstruction(output::Instruction* instruction)
{
    _tw->write(instructionTypeToString.get(instruction->type));
    if (instruction->operand1)
    {
        writeOperand(instruction->operand1);
    }
    if (instruction->operand2)
    {
        writeOperand(instruction->operand2);
    }
    _tw->writeCharacter(';');
    _tw->newline();
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

TextWriter*
BaselineWriter::createTextWriter()
{
    auto textWriter = new TextWriter();
    _textWriters.emplace(textWriter);
    return textWriter;
}


}