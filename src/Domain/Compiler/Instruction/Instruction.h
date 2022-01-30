#ifndef ELET_INSTRUCTION_H
#define ELET_INSTRUCTION_H


#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Syntax/Syntax.h>
#include <Domain/Compiler/CompilerTypes.h>
#include <variant>


namespace elet::domain::compiler::ast
{
    struct Declaration;
    struct VariableDeclaration;
    enum class BinaryOperatorKind;

    namespace type
    {
        enum TypeSize : uint64_t;
    }
}


namespace elet::domain::compiler::instruction::output
{
    struct Constant;
    struct Routine;
    struct ParameterDeclaration;
    struct ArgumentDeclaration;
    struct String;
    struct SubtractImmediateToRegisterInstruction;
    enum class OperandRegister;


    struct ImmediateValue
    {
        IntegerType
        integerType;

        uint64_t
        value;


        ImmediateValue(IntegerKind integerKind, uint64_t value):
            integerType(integerKind),
            value(value)
        {

        }

        ImmediateValue(TypeKind typeKind, uint64_t value):
            ImmediateValue(static_cast<IntegerKind>(typeKind), value)
        {

        }

        ImmediateValue(IntegerType type, uint64_t value):
            ImmediateValue(type.kind, value)
        {

        }
    };


    typedef std::variant<std::monostate, OperandRegister, ImmediateValue> CanonicalExpression;
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


    enum class OperandRegister
    {
        Return,
        Scratch0,
        Scratch1,
        Scratch2,
        FramePointer,
        StackPointer,
        LinkRegister,

        Arg0,
        Arg1,
        Arg2,
        Arg3,

        Argument = Arg0,
    };


    enum class InstructionKind
    {
        Push,
        Pop,
        Load,
        StoreImmediate,
        StoreRegister,
        AddRegisterToRegister,
        AddAddressToRegister,
        AddImmediateToRegister,
        SubtractRegisterToRegister,
        SubtractImmediateToRegister,
        SubtractRegisterToAddress,
        MultiplySignedRegisterToRegister,
        MultiplySignedAddressToRegister,
        DivideUnsignedRegisterToRegister,
        DivideUnsignedAddressToRegister,
        DivideSignedRegisterToRegister,
        DivideSignedAddressToRegister,
        ModuloUnsignedRegisterToRegister,
        ModuloUnsignedAddressToRegister,
        ModuloSignedRegisterToRegister,
        ModuloSignedAddressToRegister,
        AndRegisterToRegister,
        XorRegisterToRegister,
        OrRegisterToRegister,
        Call,
        MoveImmediate,
        MoveRegister,
        MoveAddress,
        ArgumentDeclaration,
        ParameterDeclaration,
        Return,
        ResetRegister,
    };


    struct Instruction
    {
        InstructionKind
        kind;

        RegisterSize
        registerSize;

        Instruction(InstructionKind kind, RegisterSize registerSize):
            kind(kind),
            registerSize(registerSize)
        { }
    };


//    struct PushInstruction : Instruction
//    {
//        OperandRegister
//        target;
//
//        uint64_t
//        stackSize = 0;
//
//        PushInstruction(OperandRegister target):
//            Instruction(InstructionKind::Push),
//            target(target)
//        { }
//    };
//
//
//    struct PopInstruction : Instruction
//    {
//        OperandRegister
//        target;
//
//        PopInstruction(OperandRegister target):
//            Instruction(InstructionKind::Pop),
//            target(target)
//        { }
//    };


    struct LoadInstruction : Instruction
    {
        uint64_t
        stackOffset;

        OperandRegister
        destination;

        LoadInstruction(OperandRegister destination, uint64_t stackOffset, RegisterSize registerSize):
            Instruction(InstructionKind::Load, registerSize),
            destination(destination),
            stackOffset(stackOffset)
        { }
    };


    struct MoveImmediateInstruction : Instruction
    {
        OperandRegister
        destination;

        uint64_t
        value;

        std::optional<output::Constant*>
        constant;

        MoveImmediateInstruction(OperandRegister destination, uint64_t value, RegisterSize registerSize):
            Instruction(InstructionKind::MoveImmediate, registerSize),
            destination(destination),
            value(value)
        { }
    };


    struct MoveAddressToRegisterInstruction : Instruction
    {
        OperandRegister
        destination;

        uint64_t
        offset;

        std::variant<std::monostate, output::Constant*, output::String*>
        constant;

        MoveAddressToRegisterInstruction(OperandRegister destination, uint64_t offset, RegisterSize registerSize):
            Instruction(InstructionKind::MoveAddress, registerSize),
            destination(destination),
            offset(offset)
        { }
    };


    struct MoveRegisterToRegisterInstruction : Instruction
    {
        OperandRegister
        destination;

        OperandRegister
        target;

        MoveRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, RegisterSize registerSize):
            Instruction(InstructionKind::MoveRegister, registerSize),
            destination(destination),
            target(target)
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
            Instruction(InstructionKind::Call, RegisterSize::None)
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
        stringTableIndexAddress = 0;

        Routine(RoutineKind kind):
            kind(kind)
        { }
    };


    struct InternalRoutine : Routine
    {
        // Used in Pe32 to relocate symbol index in an internal routine
        List<uint64_t>
        symbolTableIndexRelocationList;

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
        List<uint64_t>
        relocationAddressList;

        List<uint64_t>
        symbolTableIndexRelocationList;

        Utf8StringView
        name;

        uint16_t
        libraryOrdinal;

        uint64_t
        stubAddress;

        uint64_t
        stubHelperAddress;

        ExternalRoutine(Utf8StringView name):
            name(name),
            Routine(RoutineKind::External)
        { }
    };


    struct RelocationPlaceholder
    {
        uint32_t
        offset;

        // aarch64: adrp instruction offset. It's used to calculate mod 4k page size.
        // pe32: string symbol relocation offset
        uint32_t
        value1 = 0;

        // pe32: rdata relocation offset
        uint32_t
        value2 = 0;

        // aarch64: string offset. Since, relocation is required after the text segment is placed in the object file.
        // pe32: string table relocation offset
        uint32_t
        value3 = 0;

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

        bool
        inferReturnZero;

        uint8_t
        parameterSize = 0;

        uint8_t
        calleeParameterSize = 0;

        uint8_t
        localVariableSize = 0;

        uint64_t
        stackSize = 0;

        uint64_t
        codeSize = 0;

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
        uint64_t
        stackOffset;

        MemoryAllocation(InstructionKind kind, RegisterSize registerSize):
        Instruction(kind, registerSize)
        { }

        MemoryAllocation(InstructionKind kind, uint64_t& stackOffset, RegisterSize registerSize):
            Instruction(kind, registerSize),
            stackOffset(stackOffset + static_cast<uint64_t>(registerSize))
        {
            stackOffset += static_cast<uint64_t>(registerSize);
        }
    };


    struct StoreRegisterInstruction : MemoryAllocation
    {
        OperandRegister
        target;

        StoreRegisterInstruction(OperandRegister target, uint64_t& stackOffset, RegisterSize allocationSize):
            MemoryAllocation(InstructionKind::StoreRegister, stackOffset, allocationSize),
            target(target)
        {

        }
    };


    struct StoreImmediateInstruction : MemoryAllocation
    {
        uint64_t
        value;

        std::optional<Constant*>
        constant;

        StoreImmediateInstruction(uint64_t value, uint64_t& stackOffset, RegisterSize allocationSize):
            MemoryAllocation(InstructionKind::StoreImmediate, stackOffset, allocationSize),
            value(value)
        {

        }
    };


    struct ParameterDeclaration : MemoryAllocation
    {
        unsigned int
        index;

        ParameterDeclaration(unsigned int index, RegisterSize registerSize):
            MemoryAllocation(InstructionKind::ParameterDeclaration, registerSize),
            index(index)
        { }
    };


    typedef std::variant<std::size_t, String*, ParameterDeclaration*, StoreRegisterInstruction*> ArgumentValue;


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


    enum class ScratchRegisterIndex
    {
        _0,
        _1,
    };


    struct ScratchRegister : Operand
    {
        ScratchRegisterIndex
        index;

        Operation*
        operation;

        ScratchRegister(ScratchRegisterIndex index):
            Operand(OperandKind::ScratchRegister),
            index(index)
        { }
    };


    typedef std::variant<std::monostate, OperandRegister, ImmediateValue> CanonicalExpression;


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

        ast::Symbol*
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


    struct ReturnInstruction : Instruction
    {
        ReturnInstruction():
            Instruction(InstructionKind::Return, RegisterSize::None)
        { }
    };


    struct ResetRegisterInstruction : Instruction
    {
        OperandRegister
        target;

        ResetRegisterInstruction(OperandRegister target):
            Instruction(InstructionKind::ResetRegister, RegisterSize::None),
            target(target)
        { }
    };
}

#include "Instruction.Constant.h"
#include "Instruction.Arithmetic.h"


#endif //ELET_INSTRUCTION_H