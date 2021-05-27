#include "AssemblyWriterInterface.h"


namespace elet::domain::compiler::instruction::output
{


AssemblyWriterInterface::AssemblyWriterInterface(List<std::uint8_t>* output):
    _output(output)
{

}


List<std::uint8_t>*
AssemblyWriterInterface::getOutput()
{
    return _output;
}


std::size_t
AssemblyWriterInterface::getOffset()
{
    return _currentOffset;
}


void
AssemblyWriterInterface::writeCStringSection()
{
    for (const auto& string : _strings)
    {
        _bw->writeDoubleWordAtAddress(_currentOffset, string->relocationAddress);

        for (const auto s : string->value)
        {
            _bw->writeByte(static_cast<std::uint8_t>(s));
        }
        _bw->writeByte(0);
    }
}



bool
AssemblyWriterInterface::hasExternalRoutines()
{
    return !_externalRoutines.isEmpty();
}


std::uint32_t
AssemblyWriterInterface::getExternRoutinesSize() const
{
    return _externalRoutines.size();
}


bool
AssemblyWriterInterface::hasStrings()
{
    return !_strings.isEmpty();
}


}