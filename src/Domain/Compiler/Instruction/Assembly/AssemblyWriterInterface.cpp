#include "AssemblyWriterInterface.h"


namespace elet::domain::compiler::instruction::output
{


AssemblyWriterInterface::AssemblyWriterInterface(List<std::uint8_t>* output):
    _output(output),
    bw(new ByteWriter(output, &_offset))
{

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
//    writeFunctionPrologue(function);
//    writeFunctionParameters(function);
    writeFunctionInstructions(function);
//    if (function->isStartFunction)
//    {
//        writeStartFunctionEpilogue(function);
//    }
//    writeFunctionEpilogue(function);
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
    internalRoutines.add(function);
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
    return externalRoutines.size();
}


void
AssemblyWriterInterface::writePadding(size_t amount)
{
    for (size_t i = 0; i < amount; ++i)
    {
        bw->writeByte(0);
    }
}


uint64_t
AssemblyWriterInterface::getStackSizeFromFunctionParameters(const FunctionRoutine* routine)
{
    uint64_t stackOffset = 0;
    for (unsigned int i = 0; i < routine->parameters.size(); ++i)
    {
        auto parameter = routine->parameters[i];
        if (i < _callingConvention.registers.size())
        {
            stackOffset += parameter->size;
        }
    }
    uint64_t rest = stackOffset % 16;
    if (rest != 0)
    {
        stackOffset += 16 - rest;
    }
    return stackOffset;
}


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
            case InstructionKind::MoveRegister:
                writeMoveRegisterInstruction(reinterpret_cast<MoveRegisterInstruction*>(instruction), function);
                break;
            case InstructionKind::MoveAddress:
                writeMoveAddressInstruction(reinterpret_cast<MoveAddressInstruction*>(instruction), function);
                break;
            case InstructionKind::Push:
                writePushInstruction(reinterpret_cast<PushInstruction*>(instruction), function);
                break;
            case InstructionKind::Pop:
                writePopInstruction(reinterpret_cast<PopInstruction*>(instruction), function);
                break;
            case InstructionKind::AddRegister:
                writeAddRegisterInstruction(reinterpret_cast<AddRegisterInstruction*>(instruction), function);
                break;
            case InstructionKind::AddImmediate:
                writeAddImmediateInstruction(reinterpret_cast<AddImmediateInstruction*>(instruction), function);
                break;
            case InstructionKind::SubtractImmediate:
                writeSubtractImmediateInstruction(reinterpret_cast<SubtractImmediateInstruction*>(instruction), function);
                break;
            case InstructionKind::Return:
                writeReturnInstruction(reinterpret_cast<ReturnInstruction*>(instruction), function);
                break;
            case InstructionKind::ResetRegister:
                writeResetRegisterInstruction(reinterpret_cast<ResetRegisterInstruction*>(instruction), function);
                break;
            default:
                throw std::runtime_error("Unknown local instruction.");
        }
        instructionIterator.next();
    }
}



void
AssemblyWriterInterface::writeMoveAddressInstruction(MoveAddressInstruction* moveAddressInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}



void
AssemblyWriterInterface::writeAddRegisterInstruction(AddRegisterInstruction* addRegisterInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writePushInstruction(PushInstruction* pushInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writePopInstruction(PopInstruction* popInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writeMoveRegisterInstruction(MoveRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented method.");
}


void
AssemblyWriterInterface::writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented");
}


void
AssemblyWriterInterface::writeResetRegisterInstruction(ResetRegisterInstruction* resetResetRegisterInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented");
}


void
AssemblyWriterInterface::writeSubtractImmediateInstruction(SubtractImmediateInstruction* subtractImmediateInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented");
}


void
AssemblyWriterInterface::writeAddImmediateInstruction(AddImmediateInstruction* subtractImmediateInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented");
}


bool
AssemblyWriterInterface::nextInstructionIs(InstructionKind kind) const
{
    return _nextInstruction && _nextInstruction->kind == kind;
}


AssemblyWriterInterface::InstructionIterator::InstructionIterator(List<output::Instruction*>& instructions):
    _instructions(instructions)
{

}


output::Instruction*
AssemblyWriterInterface::InstructionIterator::next()
{
    _cursor += 1;
    return _instructions[_cursor];
}


output::Instruction*
AssemblyWriterInterface::InstructionIterator::peek()
{
    return _instructions[_cursor + 1];
}


void
AssemblyWriterInterface::writeInstructionsPadding(FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented.");
}


}