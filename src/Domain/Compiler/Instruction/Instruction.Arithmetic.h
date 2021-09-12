#ifndef ELET_INSTRUCTION_ARITHMETIC_H
#define ELET_INSTRUCTION_ARITHMETIC_H


#include "./Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    struct AddRegisterToRegisterInstruction : Instruction
    {
        AddRegisterToRegisterInstruction():
            Instruction(InstructionKind::AddRegisterToRegister)
        { }
    };
}


#endif //ELET_INSTRUCTION_ARITHMETIC_H
