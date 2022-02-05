#include "AssemblyWriterInterface.h"


namespace elet::domain::compiler::instruction::output
{
    AssemblyWriterInterface::AssemblyWriterInterface(List<std::uint8_t>* output):
        _output(output),
        bw(new ByteWriter(output, &_offset))
    {

    }


    void
    AssemblyWriterInterface::setCallingConvention(CallingConvention callingConvention)
    {
        assert("You have not implemented setCallingConvention.");
    }


    void
    AssemblyWriterInterface::writeFunction(FunctionRoutine* function)
    {
        if (function->hasWrittenOutput)
        {
            writeFunctionRelocationAddresses(function);
            return;
        }
        function->offset = _offset;
        writeFunctionRelocationAddresses(function);
        writeFunctionPrologue(function);
        writeFunctionInstructions(function);
        writeFunctionEpilogue(function);
        writeInstructionsPadding(function);
        for (const auto& subRoutine : function->subRoutines)
        {
            writeFunction(subRoutine);
        }
        function->hasWrittenOutput = true;
        if (function->isStartFunction)
        {
            exportedRoutines.add(function);
        }
        internalRoutineList.add(function);
    }


    //void
    //AssemblyWriterInterface::writeFunctionParameters(FunctionRoutine* routine)
    //{
    //    for (unsigned int i = 0; i < routine->parameters.size(); ++i)
    //    {
    //        auto parameter = routine->parameters[i];
    //        if (i < _callingConvention.registers.size())
    //        {
    //            writeParameter(parameter, i, routine);
    //        }
    //    }
    //}


    List<std::uint8_t>*
    AssemblyWriterInterface::getOutput()
    {
        return _output;
    }


    uint64_t
    AssemblyWriterInterface::getOffset()
    {
        return _offset;
    }


    std::uint32_t
    AssemblyWriterInterface::getExternRoutinesSize() const
    {
        return externalRoutineList.size();
    }


    void
    AssemblyWriterInterface::writePadding(size_t amount)
    {
        for (size_t i = 0; i < amount; ++i)
        {
            bw->writeByte(0);
        }
    }


//    uint64_t
//    AssemblyWriterInterface::getStackSizeFromFunctionParameters(const FunctionRoutine* routine)
//    {
//        uint64_t stackOffset = 0;
//        for (unsigned int i = 0; i < routine->parameters.size(); ++i)
//        {
//            auto parameter = routine->parameters[i];
//            if (i < _parameterRegisters.registers.size())
//            {
//                stackOffset += static_cast<int>(parameter->allocationSize);
//            }
//        }
//        uint64_t rest = stackOffset % 16;
//        if (rest != 0)
//        {
//            stackOffset += 16 - rest;
//        }
//        return stackOffset;
//    }


    void
    AssemblyWriterInterface::relocateCStrings(uint64_t textSegmentStartOffset)
    {
        // Optional, Implement this function if the object file requires relocation of C Strings.
    }

    void
    AssemblyWriterInterface::writeFunctionInstructions(FunctionRoutine* function)
    {
        auto instructionIterator = function->instructions.begin();
        _instructionIterator = &instructionIterator;
        auto end = function->instructions.end();
        while (instructionIterator != end)
        {
            output::Instruction* instruction = *instructionIterator;
            auto peek = instructionIterator.peek();
            if (peek != end)
            {
                _nextInstruction = *peek;
            }
            else
            {
                _nextInstruction = nullptr;
            }
            switch (instruction->kind)
            {
                case InstructionKind::Call:
                    writeCallInstruction(reinterpret_cast<CallInstruction*>(instruction), function);
                    break;
                case InstructionKind::Load:
                    writeLoadInstruction(reinterpret_cast<LoadInstruction*>(instruction), function);
                    break;
                case InstructionKind::StoreImmediate:
                    writeStoreImmediateInstruction(reinterpret_cast<StoreImmediateInstruction*>(instruction), function);
                    break;
                case InstructionKind::StoreRegister:
                    writeStoreRegisterInstruction(reinterpret_cast<StoreRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::MoveImmediate:
                    writeMoveImmediateInstruction(reinterpret_cast<MoveImmediateInstruction*>(instruction), function);
                    break;
                case InstructionKind::MoveRegister:
                    writeMoveRegisterToRegisterInstruction(
                        reinterpret_cast<MoveRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::MoveAddress:
                    writeMoveAddressInstruction(reinterpret_cast<MoveAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::AddRegisterToRegister:
                    writeAddRegisterInstruction(reinterpret_cast<AddRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::AddImmediateToRegister:
                    writeAddImmediateInstruction(reinterpret_cast<AddImmediateToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::AddAddressToRegister:
                    writeAddAddressToRegisterInstruction(
                        reinterpret_cast<AddAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::MultiplySignedAddressToRegister:
                    writeMultiplySignedRegisterAddressInstruction(reinterpret_cast<MultiplySignedAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::MultiplySignedRegisterToRegister:
                    writeMultiplySignedRegisterToRegisterInstruction(reinterpret_cast<MultiplySignedRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::DivideUnsignedAddressToRegister:
                    writeDivideUnsignedRegisterAddressInstruction(reinterpret_cast<DivideUnsignedAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::DivideSignedAddressToRegister:
                    writeDivideSignedRegisterAddressInstruction(reinterpret_cast<DivideSignedAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::DivideSignedRegisterToRegister:
                    writeDivideSignedRegisterToRegisterInstruction(
                        reinterpret_cast<DivideSignedRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::DivideUnsignedRegisterToRegister:
                    writeDivideUnsignedRegisterToRegisterInstruction(
                        reinterpret_cast<DivideUnsignedRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::ModuloUnsignedAddressToRegister:
                    writeModuloUnsignedRegisterAddressInstruction(reinterpret_cast<ModuloUnsignedAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::ModuloUnsignedRegisterToRegister:
                    writeModuloUnsignedRegisterToRegisterInstruction(reinterpret_cast<ModuloUnsignedRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::ModuloSignedAddressToRegister:
                    writeModuloSignedRegisterAddressInstruction(reinterpret_cast<ModuloSignedAddressToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::ModuloSignedRegisterToRegister:
                    writeModuloSignedRegisterToRegisterInstruction(reinterpret_cast<ModuloSignedRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::SubtractImmediateToRegister:
                    writeSubtractImmediateInstruction(reinterpret_cast<SubtractImmediateToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::SubtractRegisterToAddress:
                    writeSubtractRegisterAddressInstruction(reinterpret_cast<SubtractRegisterToAddressInstruction*>(instruction), function);
                    break;
                case InstructionKind::SubtractRegisterToRegister:
                    writeSubtractRegisterToRegisterInstruction(reinterpret_cast<SubtractRegisterToRegisterInstruction*>(instruction), function);
                    break;
                case InstructionKind::Return:
                    writeReturnInstruction(reinterpret_cast<ReturnInstruction*>(instruction), function);
                    break;
                default:
                    throw std::runtime_error("Unknown local instruction.");
            }
            instructionIterator.next();
        }
    }


    void
    AssemblyWriterInterface::writeAddAddressToRegisterInstruction(AddAddressToRegisterInstruction* addRegisterAddressInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeAddAddressToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeMultiplySignedRegisterAddressInstruction(MultiplySignedAddressToRegisterInstruction* multiplyRegisterAddressInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeMultiplySignedRegisterAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeMultiplySignedRegisterToRegisterInstruction(MultiplySignedRegisterToRegisterInstruction* pInstruction, FunctionRoutine* pRoutine)
    {
        throw std::runtime_error("Not implemented \"writeMultiplySignedRegisterToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeDivideUnsignedRegisterAddressInstruction(DivideUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeDivideUnsignedRegisterAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeDivideSignedRegisterAddressInstruction(DivideSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeDivideSignedRegisterAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeDivideSignedRegisterToRegisterInstruction(DivideSignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeDivideSignedRegisterToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeDivideUnsignedRegisterToRegisterInstruction(DivideUnsignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeDivideUnsignedRegisterToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeModuloSignedRegisterToRegisterInstruction(ModuloSignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeModuloSignedRegisterToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeModuloUnsignedRegisterToRegisterInstruction(ModuloUnsignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeModuloUnsignedRegisterToRegisterInstruction\" method.");
    }

    void
    AssemblyWriterInterface::writeModuloUnsignedRegisterAddressInstruction(ModuloUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeModuloUnsignedRegisterAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeModuloSignedRegisterAddressInstruction(ModuloSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeModuloSignedRegisterAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeMoveAddressInstruction(MoveAddressToRegisterInstruction* moveAddressInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeMoveAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeAddRegisterInstruction(AddRegisterToRegisterInstruction* addRegisterInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeAddRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeStoreRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeStoreImmediateInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
    {
        throw std::runtime_error("Not implemented \"writeCallInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeLoadInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeMoveRegisterToRegisterInstruction(MoveRegisterToRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeMoveRegisterToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeSubtractImmediateInstruction(SubtractImmediateToRegisterInstruction* subtractImmediateInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeSubtractImmediateInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeSubtractRegisterAddressInstruction(SubtractRegisterToAddressInstruction* subtractRegisterAddressInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeSubtractRegisterAddressInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeAddImmediateInstruction(AddImmediateToRegisterInstruction* subtractImmediateInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeAddImmediateInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeSubtractRegisterToRegisterInstruction(SubtractRegisterToRegisterInstruction* pInstruction, FunctionRoutine* pRoutine)
    {
        throw std::runtime_error("Not implemented \"writeSubtractRegisterToRegisterInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeFunctionPrologue(FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeFunctionPrologue\" method.");
    }


    void
    AssemblyWriterInterface::writeFunctionEpilogue(FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeFunctionEpilogue\" method.");
    }


    void
    AssemblyWriterInterface::writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeReturnInstruction\" method.");
    }


    void
    AssemblyWriterInterface::writeMoveImmediateInstruction(MoveImmediateInstruction* moveImmediateInstruction, FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeMoveImmediateInstruction\" method.");
    }


    bool
    AssemblyWriterInterface::nextInstructionIs(InstructionKind kind) const
    {
        return _nextInstruction && _nextInstruction->kind == kind;
    }


    void
    AssemblyWriterInterface::writeInstructionsPadding(FunctionRoutine* function)
    {
        throw std::runtime_error("Not implemented \"writeInstructionsPadding\" method.");
    }


    List<String*>
    AssemblyWriterInterface::getStrings() const
    {
        return _strings;
    }
}