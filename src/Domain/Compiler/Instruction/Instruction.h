#ifndef ELET_INSTRUCTION_H
#define ELET_INSTRUCTION_H


#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Syntax/Syntax.h>
#include "Domain/Compiler/Instruction/Syntax.h"
#include <variant>


namespace elet::domain::compiler::ast
{
    struct Declaration;
    enum class BinaryOperatorKind;

    namespace type
    {
        enum TypeSize : uint64_t;
    }
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
struct String;
struct ScratchRegister;
typedef std::variant<int64_t, uint64_t> ImmediateValue;
typedef std::variant<ScratchRegister*, ImmediateValue> CanonicalExpression;
using namespace embedded;
using namespace foundation;


enum class OperandKind : std::uint64_t
{
    Unknown,
    Int32,
    Int64,
    Register,
    ScratchRegister,
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
    VariableDeclaration,
    TemporaryVariableDeclaration,
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

    uint64_t
    offset = 0;

    uint32_t
    symbolTableIndex;

    uint64_t
    stringTableIndexAddress;

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
    List<size_t>
    relocationAddresses;

    Utf8StringView
    name;

    uint16_t
    libraryOrdinal;

    uint64_t
    stubAddress;

    uint64_t
    stubHelperAddress;

    ExternalRoutine(Utf8StringView& name):
        name(name),
        Routine(RoutineKind::External)
    { }
};


struct RelocationPlaceholder
{
    uint32_t
    offset;

    // aarch64: adrp instruction offset. It's used to calculate mod 4k page size.
    uint32_t
    value1;

    // aarch64: string offset. Since, relocation is required after the text segment is placed in the object file.
    uint32_t
    value2;

    RelocationPlaceholder(uint32_t offset, uint32_t value1):
        offset(offset),
        value1(value1),
        value2(0)
    { }

    RelocationPlaceholder(uint32_t offset, uint32_t value1, uint32_t value2):
        offset(offset),
        value1(value1),
        value2(value2)
    { }
};


struct FunctionRoutine : InternalRoutine
{
    Utf8StringView
    name;

    List<ParameterDeclaration*>
    parameters;

    bool
    hasWrittenOutput = false;

    List<FunctionRoutine*>
    subRoutines;

    List<RelocationPlaceholder>
    relocationAddresses;

    bool
    isStartFunction;

    uint64_t
    stackSize;

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


struct MemoryAllocation : Instruction
{
    ast::type::TypeSize
    size;

    std::size_t
    stackOffset;

    MemoryAllocation(InstructionKind kind, ast::type::TypeSize size):
        Instruction(kind),
        size(size)
    { }
};


struct VariableDeclaration : MemoryAllocation
{
    CanonicalExpression
    expression;

    VariableDeclaration(ast::type::TypeSize size):
        MemoryAllocation(InstructionKind::VariableDeclaration, size)
    { }

    VariableDeclaration(InstructionKind kind, ast::type::TypeSize size):
        MemoryAllocation(kind, size)
    { }
};


struct ParameterDeclaration : MemoryAllocation
{
    unsigned int
    index;

    ParameterDeclaration(unsigned int index, ast::type::TypeSize size):
        MemoryAllocation(InstructionKind::ParameterDeclaration, size),
        index(index)
    { }
};


typedef std::variant<std::size_t, String*, ParameterDeclaration*, VariableDeclaration*> ArgumentValue;


struct ArgumentDeclaration : VariableDeclaration
{
    ArgumentValue
    value;

    unsigned int
    index;

    ArgumentDeclaration(unsigned int index, ast::type::TypeSize size, const ArgumentValue& value):
        value(value),
        index(index),
        VariableDeclaration(InstructionKind::ArgumentDeclaration, size)
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

enum class OperationKind
{
    ImmediateToMemory,
};


struct Operation
{
    OperationKind
    kind;

    ast::BinaryOperatorKind
    _operator;

    Operation(OperationKind kind, ast::BinaryOperatorKind _operator):
        _operator(_operator),
        kind(kind)
    {

    }
};


struct ScratchRegister : Operand
{
    Operation*
    operation;

    ScratchRegister():
        Operand(OperandKind::ScratchRegister)
    { }
};


typedef std::variant<ScratchRegister*, ImmediateValue> CanonicalExpression;


struct ImmediateToMemoryOperation : Operation
{
    ScratchRegister*
    scratchRegister;

    ImmediateValue
    immediate;

    ImmediateToMemoryOperation(ScratchRegister* scratchRegister, ImmediateValue immediate, ast::BinaryOperatorKind _operator):
        Operation(OperationKind::ImmediateToMemory, _operator),
        scratchRegister(scratchRegister),
        immediate(immediate)
    {

    }
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

#include "Instruction.Constant.h"

#endif //ELET_INSTRUCTION_H
