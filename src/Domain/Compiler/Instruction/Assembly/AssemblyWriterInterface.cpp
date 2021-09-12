#include "AssemblyWriterInterface.h"


namespace elet::domain::compiler::instruction::output
{


AssemblyWriterInterface::AssemblyWriterInterface(List<std::uint8_t>* output):
    _output(output),
    bw(new ByteWriter(output, &_offset))
{

}


void
AssemblyWriterInterface::writeFunction(FunctionRoutine* routine)
{
    if (routine->hasWrittenOutput)
    {
        writeFunctionRelocationAddresses(routine);
        return;
    }
    routine->offset = _offset;
    writeFunctionRelocationAddresses(routine);
    uint64_t stackSize = getStackSizeFromFunctionParameters(routine);
    uint64_t stackOffset = 0;
    uint64_t routineStackSize = 0;
    routineStackSize += writeFunctionPrologue(stackSize);
    writeFunctionParameters(routine, routineStackSize);
    routineStackSize += writeFunctionInstructions(routine, stackOffset);
    if (routine->isStartFunction)
    {
        bw->writeByte(OneByteOpCode::Xor_Ev_Gv);
        bw->writeByte(ModBits::Mod3 | OpCodeRegister::Reg_RAX | RmBits::Rm0);
        routineStackSize += 2;
    }
    writeFunctionEpilogue(stackSize, routineStackSize);
    for (const auto& subRoutine : routine->subRoutines)
    {
        writeFunction(subRoutine);
    }
    routine->hasWrittenOutput = true;
    if (routine->isStartFunction)
    {
        exportedRoutines.add(routine);
    }
    internalRoutines.add(routine);
}

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


}