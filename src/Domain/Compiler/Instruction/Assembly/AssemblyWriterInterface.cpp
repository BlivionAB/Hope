#include "AssemblyWriterInterface.h"


namespace elet::domain::compiler::instruction::output
{


AssemblyWriterInterface::AssemblyWriterInterface(List<std::uint8_t>* output):
    _output(output),
    bw(new ByteWriter(output, &_offset))
{

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