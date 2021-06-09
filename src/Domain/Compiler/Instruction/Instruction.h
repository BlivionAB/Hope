#ifndef ELET_INSTRUCTION_H
#define ELET_INSTRUCTION_H


#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Syntax/Syntax.h>
#include "Domain/Compiler/Instruction/Syntax.h"
#include "Instruction.Constant.h"
#include <variant>


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
struct ParameterDeclaration;
struct ArgumentDeclaration;
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


enum class InstructionKind
{
    Call,
    ArgumentDeclaration,
    ParameterDeclaration,
    LocalVariableDeclaration,
};


struct Instruction
{
    InstructionKind
    kind;

    Instruction(InstructionKind kind):
        kind(kind)
    { }
};


struct CallInstruction : Instruction
{
    List<ArgumentDeclaration*>
    arguments;

    Routine*
    routine;

    std::uint64_t
    offset;

    CallInstruction():
        Instruction(InstructionKind::Call)
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
    size_t
    address;

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


//struct Relocation
//{
//    std::size_t
//    start;
//
//    std::size_t
//    end;
//};


struct ExternalRoutine : Routine
{
    List<size_t>
    relocationAddresses;

    Utf8StringView
    name;

    uint16_t
    libraryOrdinal;

    uint64_t
    stubAddress;

    uint64_t
    stringTableIndexAddress;

    uint64_t
    stubHelperAddress;

    uint32_t
    symbolTableIndex;

    ExternalRoutine(Utf8StringView& name):
        name(name),
        Routine(RoutineKind::External)
    { }
};


struct FunctionRoutine : InternalRoutine
{
    bool
    isStartFunction;

    Utf8StringView
    name;

    List<ParameterDeclaration*>
    parameters;

    uint64_t
    offset;

    bool
    hasWrittenOutput = false;

    List<FunctionRoutine*>
    subRoutines;

    List<std::pair<std::size_t, std::size_t>>
    relocationAddresses;

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


struct VariableDeclaration : Instruction
{
    unsigned int
    index;

    std::size_t
    size;

    std::size_t
    stackOffset;

    VariableDeclaration(InstructionKind kind, unsigned int index, std::size_t size):
        index(index),
        size(size),
        Instruction(kind)
    { }
};


struct ParameterDeclaration : VariableDeclaration
{
    ParameterDeclaration(unsigned int index, std::size_t size):
        VariableDeclaration(InstructionKind::ParameterDeclaration, index, size)
    { }
};


struct LocalVariableDeclaration : VariableDeclaration
{
    unsigned int
    index;

    std::size_t
    size;

    LocalVariableDeclaration(unsigned int index, std::size_t size):
        VariableDeclaration(InstructionKind::LocalVariableDeclaration, index, size)
    { }
};


typedef std::variant<std::size_t, String*, ParameterDeclaration*, LocalVariableDeclaration*> ArgumentValue;

struct ArgumentDeclaration : VariableDeclaration
{
    ArgumentValue
    value;

    ArgumentDeclaration(unsigned int index, std::size_t size, const ArgumentValue& value):
        value(value),
        VariableDeclaration(InstructionKind::ArgumentDeclaration, index, size)
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
