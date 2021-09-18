#ifndef ELET_INSTRUCTION_ARITHMETIC_H
#define ELET_INSTRUCTION_ARITHMETIC_H


#include "./Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    struct AddRegisterInstruction : Instruction
    {
        AddRegisterInstruction():
            Instruction(InstructionKind::AddRegister)
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
