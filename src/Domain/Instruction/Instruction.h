#ifndef ELET_INSTRUCTION_H
#define ELET_INSTRUCTION_H


#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <Domain/Syntax.h>
#include <Domain/Compiler.h>
#include "Syntax.h"


namespace elet::domain::compiler::ast
{
    struct Declaration;
}
namespace elet::domain::compiler
{
    struct Symbol;
}

namespace elet::domain::compiler::instruction::output
{


using namespace embedded;
using namespace foundation;


enum class OperandKind : std::uint64_t
{
    Unknown,
    Int32,
    Int64,
    Register,
    Argument,
    StringReference,
    FunctionReference,
    AssemblyReference,
    VariableReference,
    Label,
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


enum class DataReferenceKind
{
    String,
};


struct DataReference : Operand
{
    Utf8StringView*
    data;

    DataReference(OperandKind kind, Utf8StringView* data):
        data(data),
        Operand(kind)
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

    ast::Declaration*
    reference;

    AssemblyReference(Utf8StringView name):
        name(name),
        Operand(OperandKind::AssemblyReference)
    { }
};


struct Int32 : Operand
{
    std::uint32_t
    value;

    Int32(std::uint32_t value):
        value(value),
        Operand(OperandKind::Int32)
    { }
};

enum class RelocationType : std::uint8_t
{
    None,
    CString,
    Constant
};

struct Int64 : Operand
{
    std::uint64_t
    value;

    Int64(std::uint64_t value):
        value(value),
        Operand(OperandKind::Int64)
    { }
};


struct RelocationOperand : Operand
{
    std::uint32_t
    dataOffset;

    std::uint32_t
    textOffset;

    Symbol*
    symbol;

    RelocationOperand(OperandKind kind):
        Operand(kind)
    { }
};


struct StringReference : RelocationOperand
{
    StringReference(std::uint32_t dataOffset):
        RelocationOperand(OperandKind::StringReference)
    {
        this->dataOffset = dataOffset;
    }
};


struct FunctionReference : RelocationOperand
{
    const Utf8StringView
    reference;

    Symbol*
    symbol;

    FunctionReference(const Utf8StringView reference):
        reference(reference),
        RelocationOperand(OperandKind::FunctionReference)
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

    List<std::uint8_t>*
    machineInstructions;

    Symbol*
    symbol;

    List<RelocationOperand*>*
    symbolicRelocations;

    List<RelocationOperand*>*
    relativeRelocations;
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
    List<Parameter*>
    parameters;
};


}


#endif //ELET_INSTRUCTION_H
