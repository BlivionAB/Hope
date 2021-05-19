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
        writeDoubleWordAtAddress(_currentOffset, string->relocationAddress);

        for (const auto s : string->value)
        {
            writeByte(static_cast<std::uint8_t>(s));
        }
        writeByte(0);
    }
}


void
AssemblyWriterInterface::writeQuadWord(std::uint64_t instruction)
{
    _currentOffset += 8;
    _output->add((std::uint8_t)instruction & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)24) & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)32) & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)40) & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)48) & (std::uint8_t)0xff);
    _output->add((std::uint8_t)(instruction >> (std::uint8_t)56) & (std::uint8_t)0xff);
}


void
AssemblyWriterInterface::writeDoubleWord(std::uint32_t instruction)
{
    _currentOffset += 4;
    _output->add(instruction & (std::uint8_t)0xff);
    _output->add((instruction >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _output->add((instruction >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _output->add((instruction >> (std::uint8_t)24) & (std::uint8_t)0xff);
}


void
AssemblyWriterInterface::writeDoubleWordAtAddress(std::uint32_t instruction, std::size_t offset)
{
    (*_output)[offset] = instruction & 0xff;
    (*_output)[offset + 1] = (instruction >> 8) & 0xff;
    (*_output)[offset + 2] = (instruction >> 16) & 0xff;
    (*_output)[offset + 3] = (instruction >> 24) & 0xff;
}


void
AssemblyWriterInterface::writeByte(std::uint8_t instruction)
{
    ++_currentOffset;
    _output->add(instruction);
}


bool
AssemblyWriterInterface::hasExternalRoutines()
{
    return !_externalRoutines.isEmpty();
}


bool
AssemblyWriterInterface::hasStrings()
{
    return !_strings.isEmpty();
}


}