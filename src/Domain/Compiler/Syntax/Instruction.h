#ifndef ELET_INSTRUCTION_H
#define ELET_INSTRUCTION_H


#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Syntax/Syntax.h>
#include <Domain/Compiler/Compiler.h>
#include "Domain/Compiler/Instruction/Syntax.h"


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

struct Routine;
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


struct Instruction
{

};


struct CallInstruction : Instruction
{
    Routine*
    routine;

    CallInstruction(Routine* routine):
        routine(routine)
    { }
};


struct Condition
{

};


struct Jump
{
    Condition*
    condition;

    Routine*
    routine;
};


enum class RoutineKind
{
    Function,
    Block,
    External,
};


struct Routine
{
    RoutineKind
    kind;

    Routine(RoutineKind kind):
        kind(kind)
    { }
};


struct InternalRoutine : Routine
{
    List<Instruction*>
    instructions;

    List<Jump*>
    jumps;

    InternalRoutine*
    next;

    InternalRoutine(RoutineKind kind):
        Routine(kind)
    { }
};


struct ExternalRoutine : Routine
{
    Utf8StringView
    name;

    ExternalRoutine(Utf8StringView& name):
        name(name),
        Routine(RoutineKind::External)
    { }
};


struct FunctionRoutine : InternalRoutine
{
    Utf8StringView
    name;

    FunctionRoutine(const Utf8StringView& name):
        name(name),
        InternalRoutine(RoutineKind::Function)
    { }
};

static int blockRoutineIndex = 0;
struct BlockRoutine : InternalRoutine
{
    std::size_t
    index;

    BlockRoutine():
        index(blockRoutineIndex++),
        InternalRoutine(RoutineKind::Block)
    { }
};

//struct FunctionRoutine
//{
//    Utf8StringView
//    name;
//
//    List<Instruction*>
//    instructions;
//
//    List<Jump*>
//    jumps;
//
//    Routine*
//    next;
//};


typedef std::variant<unsigned int, int, Utf8StringView*> ArgumentValue;

struct ArgumentDeclaration : Instruction
{
    unsigned int
    index;

    ArgumentValue
    value;

    ArgumentDeclaration(unsigned int index, const ArgumentValue& value):
        index(index),
        value(value)
    { }
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

    FunctionReference(const Utf8StringView reference):
        reference(reference),
        RelocationOperand(OperandKind::FunctionReference)
    { }
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


struct Function : FunctionRoutine
{
    List<Parameter*>
    parameters;
};


}


#endif //ELET_INSTRUCTION_H
