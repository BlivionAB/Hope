#include "ByteWriter.h"

namespace elet::domain::compiler::instruction::output
{



ByteWriter::ByteWriter(List<uint8_t>* output, size_t* offset):
    _output(output),
    _offset(offset)
{

}


void
ByteWriter::writeQuadWord(uint64_t instruction)
{
    (*_offset) += 8;
    _output->add((uint8_t)instruction & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)8) & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)16) & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)24) & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)32) & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)40) & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)48) & (uint8_t)0xff);
    _output->add((uint8_t)(instruction >> (uint8_t)56) & (uint8_t)0xff);
}


void
ByteWriter::writeDoubleWord(std::uint32_t instruction)
{
    (*_offset) += 4;
    _output->add(instruction & (std::uint8_t)0xff);
    _output->add((instruction >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _output->add((instruction >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _output->add((instruction >> (std::uint8_t)24) & (std::uint8_t)0xff);
}


void
ByteWriter::writeDoubleWordAtAddress(uint32_t instruction, size_t offset)
{
    (*_output)[offset] = instruction & 0xff;
    (*_output)[offset + 1] = (instruction >> 8) & 0xff;
    (*_output)[offset + 2] = (instruction >> 16) & 0xff;
    (*_output)[offset + 3] = (instruction >> 24) & 0xff;
}


void
ByteWriter::writeByte(std::uint8_t instruction)
{
    ++(*_offset);
    _output->add(instruction);
}


void
ByteWriter::writeString(const char* string)
{
    auto length = std::strlen(string);
    for (size_t i = 0; i < length; ++i)
        _output->add(string[i]);
}


}