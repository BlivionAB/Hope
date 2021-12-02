#ifndef ELET_INSTRUCTION_ARITHMETIC_H
#define ELET_INSTRUCTION_ARITHMETIC_H


#include "./Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    struct OperationRegisterInstruction : Instruction
    {
        OperandRegister
        destination;

        OperandRegister
        target;

        OperandRegister
        value;

        OperationRegisterInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, OperandRegister value):
            Instruction(operandKind),
            destination(destination),
            target(target),
            value(value)
        { }
    };


    struct OperationRegisterAddressInstruction : Instruction
    {
        OperandRegister
        destination;

        OperandRegister
        target;

        uint64_t
        value_stackOffset;

        uint64_t
        value_size;

        OperationRegisterAddressInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, uint64_t value_stackOffset, uint64_t value_size):
            Instruction(operandKind),
            destination(destination),
            target(target),
            value_stackOffset(value_stackOffset),
            value_size(value_size)
        { }
    };


    struct AddRegisterInstruction : OperationRegisterInstruction
    {
        AddRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::AddRegister, destination, target, value)
        { }
    };


    struct AddRegisterAddressInstruction : OperationRegisterAddressInstruction
    {
        AddRegisterAddressInstruction(OperandRegister destination, OperandRegister target, size_t value_stackOffset, size_t value_size):
            OperationRegisterAddressInstruction(InstructionKind::AddRegisterAddress, destination, target, value_stackOffset, value_size)
        { }
    };


    struct SubtractRegisterInstruction : OperationRegisterInstruction
    {
        SubtractRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::SubtractRegister, destination, target, value)
        { }
    };


    struct MultiplyRegisterInstruction : OperationRegisterInstruction
    {
        MultiplyRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::MultiplyRegister, destination, target, value)
        { }
    };


    struct DivideRegisterInstruction : OperationRegisterInstruction
    {
        DivideRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::DivideRegister, destination, target, value)
        { }
    };


    struct AndRegisterInstruction : OperationRegisterInstruction
    {
        AndRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::AndRegister, destination, target, value)
        { }
    };


    struct XorRegisterInstruction : OperationRegisterInstruction
    {
        XorRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::XorRegister, destination, target, value)
        { }
    };



    struct OrRegisterInstruction : OperationRegisterInstruction
    {
        OrRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationRegisterInstruction(InstructionKind::OrRegister, destination, target, value)
        { }
    };


    struct AddImmediateInstruction : Instruction
    {
        OperandRegister
        destination;

        uint64_t
        value;

        AddImmediateInstruction(OperandRegister destination, uint64_t value):
            Instruction(InstructionKind::AddImmediate),
            destination(destination),
            value(value)
        { }
    };


    struct SubtractImmediateInstruction : Instruction
    {
        OperandRegister
        destination;

        uint64_t
        value;

        SubtractImmediateInstruction(OperandRegister destination, uint64_t value):
            Instruction(InstructionKind::SubtractImmediate),
            destination(destination),
            value(value)
        { }
    };
}


#endif //ELET_INSTRUCTION_ARITHMETIC_H
