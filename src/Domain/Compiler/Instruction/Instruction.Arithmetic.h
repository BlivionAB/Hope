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
}


#endif //ELET_INSTRUCTION_ARITHMETIC_H
