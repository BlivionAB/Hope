#include "Optimizer.h"
#include <set>

namespace elet::domain::compiler::instruction::output
{
    Optimizer::Optimizer(Optimizer::Options options):
        _options(options)
    {

    }


    void
    Optimizer::optimizeRoutine(FunctionRoutine* functionRoutine)
    {

        if (_options.assemblyHasMultiRegisterOperands)
        {
            optimizeLoadOperationInstructions(functionRoutine);
        }
    }


    void
    Optimizer::optimizeLoadOperationInstructions(FunctionRoutine* functionRoutine)
    {
        List<Instruction*> optimizedInstructions;
        StackOffsetInstruction* currentLoadInstructions[static_cast<int>(OperandRegister::Scratch2)];
        memset(currentLoadInstructions, 0, sizeof(currentLoadInstructions)); // Scratch 2 is the max scratch register.
        bool isOperationStage = false;
        std::set<OperandRegister> loadAllocations;

        for (int i = 0; i < functionRoutine->instructions.size(); ++i)
        {
            Instruction* instruction = functionRoutine->instructions[i];
            if (isLoadInstruction(instruction) && instruction->destinationSize >= RegisterSize::Dword)
            {
                if (isOperationStage)
                {
                    memset(currentLoadInstructions, 0, sizeof(currentLoadInstructions));
                    isOperationStage = false;
                }
                StackOffsetInstruction* load = reinterpret_cast<StackOffsetInstruction*>(instruction);
                currentLoadInstructions[static_cast<unsigned int>(load->destination)] = load;
                loadAllocations.insert(load->destination);
            }
            else if (isOperationInstruction(instruction))
            {
                const OperationRegisterToRegisterInstruction* operation = reinterpret_cast<const OperationRegisterToRegisterInstruction*>(instruction);
                const StackOffsetInstruction* loadInstruction = currentLoadInstructions[static_cast<unsigned int>(operation->value)];
                if (loadInstruction != nullptr)
                {
                    for (const OperandRegister operandRegister : loadAllocations)
                    {
                        if (operandRegister == operation->value)
                        {
                            continue;
                        }
                        optimizedInstructions.add(currentLoadInstructions[static_cast<unsigned int>(operandRegister)]);
                    }
                    optimizedInstructions.add(
                        createOperationRegisterAddressInstructionFromOperationRegisterInstruction(operation, loadInstruction));
                }
                else
                {
                    optimizedInstructions.add(instruction);
                }
                isOperationStage = true;
                loadAllocations.clear();
            }
            else
            {
                optimizedInstructions.add(instruction);
            }
        }

        // TODO: Make .instructions a pointer instead of doing hard copy here.
        functionRoutine->instructions = optimizedInstructions;
    }

    bool
    Optimizer::isLoadInstruction(const Instruction* instruction) const
    {
        return (instruction->kind == InstructionKind::LoadUnsigned || instruction->kind == InstructionKind::LoadSigned);
    }


    OperationAddressToRegisterInstruction*
    Optimizer::createOperationRegisterAddressInstructionFromOperationRegisterInstruction(const OperationRegisterToRegisterInstruction* operationRegisterInstruction, const StackOffsetInstruction* loadInstruction)
    {
        switch (operationRegisterInstruction->kind)
        {
            case InstructionKind::AddRegisterToRegister:
                return new AddAddressToRegisterInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            case InstructionKind::SubtractRegisterToRegister:
                return new SubtractRegisterToAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            case InstructionKind::MultiplySignedRegisterToRegister:
                return new MultiplySignedAddressToRegisterInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            case InstructionKind::DivideUnsignedRegisterToRegister:
                return new DivideUnsignedAddressToRegisterInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            case InstructionKind::DivideSignedRegisterToRegister:
                return new DivideSignedAddressToRegisterInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            case InstructionKind::ModuloUnsignedRegisterToRegister:
                return new ModuloUnsignedAddressToRegisterInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            case InstructionKind::ModuloSignedRegisterToRegister:
                return new ModuloSignedAddressToRegisterInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, loadInstruction->destinationSize);
            default:
                throw std::runtime_error("Unknown operationRegisterInstruction");
        }
    }


    bool
    Optimizer::isOperationInstruction(const Instruction* instruction)
    {
        switch (instruction->kind)
        {
            case InstructionKind::AddRegisterToRegister:
                return true;
            case InstructionKind::SubtractRegisterToRegister:
                return true;
            case InstructionKind::MultiplySignedRegisterToRegister:
                return true;
            case InstructionKind::DivideUnsignedRegisterToRegister:
                return true;
            case InstructionKind::DivideSignedRegisterToRegister:
                return true;
            case InstructionKind::ModuloUnsignedRegisterToRegister:
                return true;
            case InstructionKind::ModuloSignedRegisterToRegister:
                return true;
            default:
                return false;
        }
    }
}