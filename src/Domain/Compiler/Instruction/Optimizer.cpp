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
        LoadInstruction* currentLoadInstructions[static_cast<int>(OperandRegister::Scratch2)]; // Scratch 2 is the max scratch register.
        bool isOperationStage = false;
        std::set<OperandRegister> loadAllocations;

        for (int i = 0; i < functionRoutine->instructions.size(); ++i)
        {
            Instruction* instruction = functionRoutine->instructions[i];
            if (instruction->kind == InstructionKind::Load)
            {
                if (isOperationStage)
                {
                    memset(currentLoadInstructions, 0, sizeof(currentLoadInstructions));
                    isOperationStage = false;
                }
                LoadInstruction* load = reinterpret_cast<LoadInstruction*>(instruction);
                currentLoadInstructions[static_cast<unsigned int>(load->destination)] = load;
                loadAllocations.insert(load->destination);
            }
            else if (isOperationInstruction(instruction))
            {
                const OperationRegisterInstruction* operation = reinterpret_cast<const OperationRegisterInstruction*>(instruction);
                const LoadInstruction* loadInstruction = currentLoadInstructions[static_cast<unsigned int>(operation->value)];
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


    OperationRegisterAddressInstruction*
    Optimizer::createOperationRegisterAddressInstructionFromOperationRegisterInstruction(const OperationRegisterInstruction* operationRegisterInstruction, const LoadInstruction* loadInstruction)
    {
        switch (operationRegisterInstruction->kind)
        {
            case InstructionKind::AddRegister:
                return new AddRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            case InstructionKind::SubtractRegister:
                return new SubtractRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            case InstructionKind::MultiplySignedRegister:
                return new MultiplySignedRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            case InstructionKind::DivideUnsignedRegister:
                return new DivideUnsignedRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            case InstructionKind::DivideSignedRegister:
                return new DivideSignedRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            case InstructionKind::ModuloUnsignedRegister:
                return new ModuloUnsignedRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            case InstructionKind::ModuloSignedRegister:
                return new ModuloSignedRegisterAddressInstruction(operationRegisterInstruction->destination, operationRegisterInstruction->target, loadInstruction->stackOffset, static_cast<uint64_t>(loadInstruction->allocationSize));
            default:
                throw std::runtime_error("Unknown operationRegisterInstruction");
        }
    }


    bool
    Optimizer::isOperationInstruction(const Instruction* instruction)
    {
        switch (instruction->kind)
        {
            case InstructionKind::AddRegister:
                return true;
            case InstructionKind::SubtractRegister:
                return true;
            case InstructionKind::MultiplySignedRegister:
                return true;
            case InstructionKind::DivideUnsignedRegister:
                return true;
            case InstructionKind::DivideSignedRegister:
                return true;
            case InstructionKind::ModuloUnsignedRegister:
                return true;
            case InstructionKind::ModuloSignedRegister:
                return true;
            default:
                return false;
        }
    }
}