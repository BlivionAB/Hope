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


void
AssemblyWriterInterface::writeCStringSection()
{
    for (const auto& string : _strings)
    {
        bw->writeDoubleWordAtAddress(_offset - (string->relocationAddress + 4), string->relocationAddress);

        for (const auto s : string->value)
        {
            bw->writeByte(static_cast<std::uint8_t>(s));
        }
        bw->writeByte(0);
    }
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


}