#ifndef ELET_INSTRUCTION_ARITHMETIC_H
#define ELET_INSTRUCTION_ARITHMETIC_H


#include "./Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    struct OperationInstruction : Instruction
    {

        OperandRegister
        destination;

        OperandRegister
        target;

        OperandRegister
        value;

        OperationInstruction(InstructionKind operandKind, OperandRegister destination, OperandRegister target, OperandRegister value):
            Instruction(operandKind),
            destination(destination),
            target(target),
            value(value)
        { }
    };


    struct AddRegisterInstruction : OperationInstruction
    {
        AddRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationInstruction(InstructionKind::AddRegister, destination, target, value)
        { }
    };


    struct MultiplyRegisterInstruction : OperationInstruction
    {
        MultiplyRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationInstruction(InstructionKind::MultiplyRegister, destination, target, value)
        { }
    };


    struct AndRegisterInstruction : OperationInstruction
    {
        AndRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationInstruction(InstructionKind::AndRegister, destination, target, value)
        { }
    };


    struct XorRegisterInstruction : OperationInstruction
    {
        XorRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationInstruction(InstructionKind::XorRegister, destination, target, value)
        { }
    };



    struct OrRegisterInstruction : OperationInstruction
    {
        OrRegisterInstruction(OperandRegister destination, OperandRegister target, OperandRegister value):
            OperationInstruction(InstructionKind::OrRegister, destination, target, value)
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
