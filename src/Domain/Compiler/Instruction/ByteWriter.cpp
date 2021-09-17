#include "ByteWriter.h"

namespace elet::domain::compiler::instruction::output
{



ByteWriter::ByteWriter(List<uint8_t>* output, uint64_t* offset):
    output(output),
    offset(offset)
{

}


void
ByteWriter::writeQuadWord(uint64_t instruction)
{
    (*offset) += 8;
    output->add((uint8_t)instruction & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)8) & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)16) & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)24) & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)32) & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)40) & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)48) & (uint8_t)0xff);
    output->add((uint8_t)(instruction >> (uint8_t)56) & (uint8_t)0xff);
}


void
ByteWriter::writeQuadWordAtAddress(uint64_t instruction, uint64_t address)
{
    (*output)[address] = instruction & 0xff;
    (*output)[address + 1] = (instruction >> 8) & 0xff;
    (*output)[address + 2] = (instruction >> 16) & 0xff;
    (*output)[address + 3] = (instruction >> 24) & 0xff;
    (*output)[address + 4] = (instruction >> 32) & 0xff;
    (*output)[address + 5] = (instruction >> 40) & 0xff;
    (*output)[address + 6] = (instruction >> 48) & 0xff;
    (*output)[address + 7] = (instruction >> 56) & 0xff;
}


void
ByteWriter::writeDoubleWord(uint32_t instruction)
{
    (*offset) += 4;
    output->add(instruction & 0xff);
    output->add((instruction >> 8) & 0xff);
    output->add((instruction >> 16) & 0xff);
    output->add((instruction >> 24) & 0xff);
}


void
ByteWriter::writeDoubleWordInFunction(uint32_t instruction, output::FunctionRoutine* function)
{
    (*offset) += 4;
    output->add(instruction & 0xff);
    output->add((instruction >> 8) & 0xff);
    output->add((instruction >> 16) & 0xff);
    output->add((instruction >> 24) & 0xff);
    function->codeSize += 4;
}


void
ByteWriter::writeDoubleWordAtAddress(uint32_t instruction, size_t offset)
{
    (*output)[offset] = instruction & 0xff;
    (*output)[offset + 1] = (instruction >> 8) & 0xff;
    (*output)[offset + 2] = (instruction >> 16) & 0xff;
    (*output)[offset + 3] = (instruction >> 24) & 0xff;
}



void
ByteWriter::writeDoubleWordAtAddress(uint32_t instruction, uint64_t offset, List<uint8_t>& output)
{
    output[offset] = instruction & 0xff;
    output[offset + 1] = (instruction >> 8) & 0xff;
    output[offset + 2] = (instruction >> 16) & 0xff;
    output[offset + 3] = (instruction >> 24) & 0xff;
}


void
ByteWriter::writeByte(uint8_t instruction)
{
    ++(*offset);
    output->add(instruction);
}


void
ByteWriter::writeByteInFunction(uint8_t instruction, output::FunctionRoutine* function)
{
    writeByte(instruction);
    function->codeSize += 1;
}


void
ByteWriter::writeByte(uint8_t instruction, uint64_t& size)
{
    ++(*offset);
    ++size;
    output->add(instruction);
}


void
ByteWriter::writeWord(uint16_t instruction)
{
    (*offset) += 2;
    output->add(instruction & (std::uint8_t)0xff);
    output->add((instruction >> (std::uint8_t)8) & (std::uint8_t)0xff);
}

void
ByteWriter::writePadding(int64_t padding)
{
    for (unsigned int i = 0; i < padding; ++i)
    {
        output->add(0);
    }
    (*offset) += padding;
}



void
ByteWriter::writeByteAtAddress(std::uint8_t instruction, std::size_t offset)
{
    (*output)[offset] = instruction;
}


uint64_t
ByteWriter::writeUleb128(uint64_t value)
{
    uint64_t size = 0;
    do {
        uint8_t byte = value & 0b01111111;
        value >>= 7;
        if (value != 0)
        {
            byte |= 0b10000000;
        }
        writeByte(byte);
        size++;
    }
    while (value != 0);
    return size;
}




size_t
ByteWriter::getUleb128Size(size_t value)
{
    size_t byteSize = 0;
    do {
        uint8_t byte = value & 0b01111111;
        value >>= 7;
        if (value != 0)
        {
            byte |= 0b10000000;
        }
        byteSize++;
    }
    while (value != 0);
    return byteSize;
}



size_t
ByteWriter::writeReverseUleb128(uint64_t value)
{
    unsigned int size = 0;
    unsigned int shift = sizeof(value) - 7;
    bool hasSeenMsb = false;
    do
    {
        size_t s = value >> shift;
        shift -= 7;
        uint8_t byte = s & 0b01111111;
        if (s != 0)
        {
            hasSeenMsb = true;
        }
        else if (hasSeenMsb)
        {
            byte |= 0b10000000;
        }
        else
        {
            continue;
        }
        writeByte(byte);
        size++;
    }
    while (shift != 0);
    return size;
}


void
ByteWriter::writeSleb128(int64_t value)
{
    bool more = true;
    bool negative = (value < 0);

    size_t size = sizeof(value);

    while (more) {
        uint8_t byte = value & 0b01111111;
        value >>= 7;

        if (negative)
        {
            value |= (~0 << (size - 7)); /* sign extend */
        }

        /* sign bit of byte is second high-order bit (0x40) */
        if ((value == 0 && (byte & 0x40) == 0/*sign bit of byte is clear*/) || (value == -1 && (byte & 0x40)/*sign bit of byte is set*/))
        {
            more = false;
        }
        else
        {
            byte |= 0b10000000;
        }
        writeByte(byte);
    }
}


void
ByteWriter::writeReverseSleb128(int64_t value)
{
    bool more = true;
    bool negative = (value < 0);

    size_t size = sizeof(value);

    while (more)
    {
        uint8_t byte = value & 0b01111111;
        value >>= 7;

        if (negative)
        {
            value |= (~0 << (size - 7)); /* sign extend */
        }

        /* sign bit of byte is second high-order bit (0x40) */
        if ((value == 0 && (byte & 0x40) == 0/*sign bit of byte is clear*/) || (value == -1 && (byte & 0x40)/*sign bit of byte is set*/))
        {
            more = false;
        }
        else
        {
            byte |= 0b10000000;
        }
        writeByte(byte);
    }
}

size_t
ByteWriter::writeString(const char* string)
{
    auto length = std::strlen(string);
    for (size_t i = 0; i < length; ++i)
    {
        output->add(string[i]);
    }
    output->add('\0');
    *offset += length + 1;
    return length + 1;
}


uint64_t
ByteWriter::writeString(const Utf8StringView& string)
{
    for (auto s : string)
    {
        output->add(static_cast<uint8_t>(s));
    }
    output->add('\0');
    uint64_t size = string.size() + 1;
    *offset += size;
    return size;

}

uint32_t
ByteWriter::getDoubleWord(uint64_t offset)
{
    uint32_t result = 0;
    result += ((*output)[offset + 0] << 0);
    result += ((*output)[offset + 1] << 8);
    result += ((*output)[offset + 2] << 16);
    result += ((*output)[offset + 3] << 24);
    return result;
}


void
ByteWriter::writeInstructionsInFunction(std::initializer_list<uint8_t> instructions, output::FunctionRoutine* function)
{
    for (const uint8_t instruction : instructions)
    {
        writeByte(instruction);
    }
    function->codeSize += instructions.size();
}

}