#ifndef ELET_INSTRUCTION_H
#define ELET_INSTRUCTION_H


#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <Domain/Syntax.h>
#include "Syntax.h"


namespace elet::domain::compiler
{
    struct Declaration;
}


namespace elet::domain::compiler::instruction::output
{


using namespace embedded;
using namespace foundation;

enum class OperandKind
{
    Unknown,
    Label,
    Register,
    VariableReference,
    AssemblyReference,
    Int,
};

struct Operand
{
    OperandKind
    kind;

    Operand(OperandKind kind):
        kind(kind)
    { }
};


struct Register : Operand
{
    std::uint8_t
    index;

    Register(unsigned int index):
        index(index),
        Operand(OperandKind::Register)
    { }
};


struct VariableReference : Operand
{
    std::size_t
    index;

    std::size_t
    offset;

    std::size_t
    size;

    bool
    isParameter;

    VariableReference(std::size_t index, std::size_t size, std::size_t offset, bool isParameter):
        index(index),
        size(size),
        offset(offset),
        isParameter(isParameter),
        Operand(OperandKind::VariableReference)
    { }
};


struct Label : Operand
{
    Utf8StringView
    symbol;

    Label(Utf8StringView& symbol):
        symbol(symbol),
        Operand(OperandKind::Label)
    { }
};


struct AssemblyReference : Operand
{
    Utf8StringView
    name;

    elet::domain::compiler::Declaration*
    reference;

    AssemblyReference(Utf8StringView name):
        name(name),
        Operand(OperandKind::AssemblyReference)
    { }
};


struct Int : Operand
{
    std::uint64_t
    value;

    Int(std::uint64_t value):
        value(value),
        Operand(OperandKind::Int)
    { }
};


struct Instruction
{
    InstructionType
    type;

    Operand*
    operand1;

    Operand*
    operand2;
};


enum class RoutineKind
{
    Unknown,
    Routine,
    Function,
};


struct Routine
{
    RoutineKind
    kind;

    Utf8StringView
    name;

    List<Instruction*>*
    instructions;
};


struct Parameter
{
    std::size_t
    size;

    std::size_t
    offset;

    unsigned int
    index;

    Utf8StringView
    symbol;

    Utf8StringView
    typeLabel;

    Parameter(
        std::size_t
        size,

        std::size_t
        offset,

        unsigned int
        index,

        Utf8StringView
        symbol,

        Utf8StringView
        label
    ):
        size(size),
        offset(offset),
        index(index),
        symbol(symbol),
        typeLabel(label)
    { }
};


struct Function : Routine
{
    List<Utf8StringView>
    parameterOrder;

    std::map<Utf8StringView, Parameter*>
    parameters;
};


}


#endif //ELET_INSTRUCTION_H
