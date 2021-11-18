#ifndef ELET_OPTIMIZER_H
#define ELET_OPTIMIZER_H


#include "Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    class Optimizer
    {
    public:

        struct Options
        {
            bool
            assemblyHasMultiRegisterOperands;
        };


        Optimizer(Options options);

        void
        optimizeRoutine(FunctionRoutine* functionRoutine);

    private:

        Options
        _options;

        bool
        isOperationInstruction(const Instruction* instruction);
    };
}


#endif //ELET_OPTIMIZER_H
