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

        OperationRegisterToRegisterInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, OperandRegister value):
            Instruction(operandKind),
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

        OperationImmediateToRegisterInstruction(InstructionKind instructionKind, OperandRegister destination, OperandRegister target, uint64_t value):
            Instruction(instructionKind),
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
        value_stackOffset;

        uint64_t
        value_size;

        OperationAddressToRegisterInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, uint64_t value_stackOffset, uint64_t value_size):
            Instruction(operandKind),
            destination(destination),
            target(target),
            value_stackOffset(value_stackOffset),
            value_size(value_size)
        { }
    };


    struct AddRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        AddRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::AddRegisterToRegister, destination, target, value)
        { }
    };


    struct AddAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        AddAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationAddressToRegisterInstruction(InstructionKind::AddAddressToRegister, destination, target, value_stackOffset, value_size)
        { }
    };


    struct SubtractRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        SubtractRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::SubtractRegisterToRegister, destination, target, value)
        { }
    };


    struct SubtractImmediateToRegisterInstruction : OperationImmediateToRegisterInstruction
    {
        SubtractImmediateToRegisterInstruction(OperandRegister destination, OperandRegister target, uint64_t value):
            OperationImmediateToRegisterInstruction(InstructionKind::SubtractImmediateToRegister, destination, target, value)
        { }
    };


    struct SubtractRegisterToAddressInstruction : OperationAddressToRegisterInstruction
    {
        SubtractRegisterToAddressInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationAddressToRegisterInstruction(InstructionKind::SubtractRegisterToAddress, destination, target, value_stackOffset, value_size)
        { }
    };


    struct MultiplySignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        MultiplySignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::MultiplySignedRegisterToRegister, destination, target, value)
        { }
    };


    struct MultiplySignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        MultiplySignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationAddressToRegisterInstruction(InstructionKind::MultiplySignedAddressToRegister, destination, target, value_stackOffset, value_size)
        { }
    };


    struct DivideUnsignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        DivideUnsignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::DivideUnsignedRegisterToRegister, destination, target, value)
        { }
    };


    struct DivideUnsignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        DivideUnsignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationAddressToRegisterInstruction(InstructionKind::DivideUnsignedAddressToRegister, destination, target, value_stackOffset, value_size)
        { }
    };


    struct DivideSignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        DivideSignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::DivideSignedRegisterToRegister, destination, target, value)
        { }
    };


    struct DivideSignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        DivideSignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
        OperationAddressToRegisterInstruction(InstructionKind::DivideSignedAddressToRegister, destination, target, value_stackOffset, value_size)
        { }
    };


    struct ModuloUnsignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        ModuloUnsignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::ModuloUnsignedRegisterToRegister, destination, target, value)
        { }
    };


    struct ModuloUnsignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        ModuloUnsignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationAddressToRegisterInstruction(InstructionKind::ModuloUnsignedAddressToRegister, destination, target, value_stackOffset, value_size)
        { }
    };


    struct ModuloSignedRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        ModuloSignedRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::ModuloSignedRegisterToRegister, destination, target, value)
        { }
    };


    struct ModuloSignedAddressToRegisterInstruction : OperationAddressToRegisterInstruction
    {
        ModuloSignedAddressToRegisterInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationAddressToRegisterInstruction(InstructionKind::ModuloSignedAddressToRegister, destination, target, value_stackOffset, value_size)
        { }
    };


    struct AndRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        AndRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::AndRegisterToRegister, destination, target, value)
        { }
    };


    struct XorRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        XorRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::XorRegisterToRegister, destination, target, value)
        { }
    };



    struct OrRegisterToRegisterInstruction : OperationRegisterToRegisterInstruction
    {
        OrRegisterToRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterToRegisterInstruction(InstructionKind::OrRegisterToRegister, destination, target, value)
        { }
    };


    struct AddImmediateToRegisterInstruction : OperationImmediateToRegisterInstruction
    {
        AddImmediateToRegisterInstruction(OperandRegister destination, OperandRegister target, uint64_t value):
        OperationImmediateToRegisterInstruction(InstructionKind::AddImmediateToRegister, destination, target, value)
        { }
    };
}


#endif //ELET_INSTRUCTION_ARITHMETIC_H
