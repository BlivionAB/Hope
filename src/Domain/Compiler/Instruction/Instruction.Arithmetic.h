#ifndef ELET_INSTRUCTION_ARITHMETIC_H
#define ELET_INSTRUCTION_ARITHMETIC_H


#include "./Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    struct OperationRegisterToRegisterInstruction : Instruction
    {
        OperandRegister
        destination;

        OperandRegister
        target;

        OperandRegister
        value;

        OperationRegisterToRegisterInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            Instruction(operandKind, registerSize),
            destination(destination),
            target(target),
            value(value)
        { }
    };


    struct OperationImmediateToRegisterInstruction : Instruction
    {
        OperandRegister
        destination;

        OperandRegister
        target;

        uint64_t
        value;

        OperationImmediateToRegisterInstruction(InstructionKind instructionKind, OperandRegister destination, OperandRegister target, uint64_t value, RegisterSize registerSize):
            Instruction(instructionKind, registerSize),
            destination(destination),
            target(target),
            value(value)
        { }
    };


    struct OperationAddressToRegisterInstruction : Instruction
    {
        OperandRegister
        destination;

        OperandRegister
        target;

        uint64_t
        stackOffset;

        OperationAddressToRegisterInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, uint64_t stackOffset, RegisterSize registerSize):
            Instruction(operandKind, registerSize),
            destination(destination),
            target(target),
            stackOffset(stackOffset)
        { }
    };


    struct AddRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        AddRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::AddRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct AddAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        AddAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::AddAddressToRegister, destination, target, stackOffset, registerSize)
        { }
    };


    struct SubtractRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        SubtractRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::SubtractRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct SubtractImmediateToRegisterInstruction : OperationImmediateToRegisterInstruction
    {
        SubtractImmediateToRegisterInstruction(OperandRegister destination, OperandRegister target, uint64_t value, RegisterSize registerSize):
            OperationImmediateToRegisterInstruction(InstructionKind::SubtractImmediateToRegister, destination, target, value, registerSize)
        { }
    };


    struct SubtractRegisterToAddressInstruction : OperationAddressToRegisterInstruction
    {
        SubtractRegisterToAddressInstruction(OperandRegister destination, OperandRegister target, size_t stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::SubtractRegisterToAddress, destination, target, stackOffset, registerSize)
        { }
    };


    struct MultiplySignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        MultiplySignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::MultiplySignedRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct MultiplySignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        MultiplySignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::MultiplySignedAddressToRegister, destination, target, stackOffset, registerSize)
        { }
    };


    struct DivideUnsignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        DivideUnsignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::DivideUnsignedRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct DivideUnsignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        DivideUnsignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::DivideUnsignedAddressToRegister, destination, target, stackOffset, registerSize)
        { }
    };


    struct DivideSignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        DivideSignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::DivideSignedRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct DivideSignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        DivideSignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, uint64_t stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::DivideSignedAddressToRegister, destination, target, stackOffset, registerSize)
        { }
    };


    struct ModuloUnsignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        ModuloUnsignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::ModuloUnsignedRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct ModuloUnsignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        ModuloUnsignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, uint64_t stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::ModuloUnsignedAddressToRegister, destination, target, stackOffset, registerSize)
        { }
    };


    struct ModuloSignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        ModuloSignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::ModuloSignedRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct ModuloSignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        ModuloSignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, RegisterSize registerSize):
            OperationAddressToRegisterInstruction(InstructionKind::ModuloSignedAddressToRegister, destination, target, value_stackOffset, registerSize)
        { }
    };


    struct AndRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        AndRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::AndRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct XorRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        XorRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::XorRegisterToRegister, destination, target, value, registerSize)
        { }
    };



    struct OrRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        OrRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value, RegisterSize registerSize):
            OperationRegisterToRegisterInstruction(InstructionKind::OrRegisterToRegister, destination, target, value, registerSize)
        { }
    };


    struct AddImmediateToRegisterInstruction : OperationImmediateToRegisterInstruction
    {
        AddImmediateToRegisterInstruction(OperandRegister destination, OperandRegister target, uint64_t value, RegisterSize registerSize):
            OperationImmediateToRegisterInstruction(InstructionKind::AddImmediateToRegister, destination, target, value, registerSize)
        { }
    };
}


#endif //ELET_INSTRUCTION_ARITHMETIC_H
