#include "Optimizer.h"


namespace elet::domain::compiler::instruction::output
{
    Optimizer::Optimizer(Optimizer::Options options):
        _options(options)
    {

    }


    void
    Optimizer::optimizeRoutine(FunctionRoutine* functionRoutine)
    {
        List<Instruction*> optimizedInstructions;

        LoadInstruction const **  loadInstructionContainer = new const LoadInstruction*[functionRoutine->stackSize];

        unsigned int availableRegister = 0;
        bool isOperationStage = false;
        for (int i = 0; i < functionRoutine->instructions.size(); ++i)
        {
            const Instruction* instruction = functionRoutine->instructions[i];

            if (_options.assemblyHasMultiRegisterOperands)
            {
                if (instruction->kind == InstructionKind::Load)
                {
                    if (isOperationStage)
                    {
                        std::memset(loadInstructionContainer, sizeof(loadInstructionContainer), 0);
                    }
                    const LoadInstruction* load = reinterpret_cast<const LoadInstruction*>(instruction);
                    loadInstructionContainer[load->stackOffset] = load;
                }
                else if (isOperationInstruction(instruction))
                {
                    const OperationInstruction* operation = reinterpret_cast<const OperationInstruction*>(instruction);
                    if (operation->target)
                }
            }
        }
    }


    bool
    Optimizer::isOperationInstruction(const Instruction* instruction)
    {
        switch (instruction->kind)
        {
            case InstructionKind::AddRegister:
                return true;
            default:
                return false;
        }
    }
}