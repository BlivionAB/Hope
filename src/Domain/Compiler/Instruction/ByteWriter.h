#ifndef ELET_BYTEWRITER_H
#define ELET_BYTEWRITER_H


#include <cstdint>
#include <Foundation/List.h>
#include <Foundation/Utf8StringView.h>
#include "Instruction.h"


namespace elet::domain::compiler::instruction::output
{


using namespace elet::foundation;


class ByteWriter
{

public:

    ByteWriter(List<uint8_t>* output, uint64_t* offset);

    void
    writeQuadWord(uint64_t instruction);

    void
    writeQuadWordAtAddress(uint64_t instruction, uint64_t);


    void
    writeDoubleWord(uint32_t instruction);

    void
    writeDoubleWordInFunction(std::uint32_t instruction, output::FunctionRoutine* function);

    void
    writeDoubleWordAtAddress(uint32_t instruction, size_t offset);

    static
    void
    writeDoubleWordAtAddress(uint32_t instruction, uint64_t offset, List<uint8_t>& output);

    void
    writeByte(uint8_t instruction);

    void
    writeByteInFunction(uint8_t instruction, output::FunctionRoutine* function);

    void
    writeInstructionsInFunction(std::initializer_list<uint8_t> instructions, output::FunctionRoutine* function);

    void
    writeByte(uint8_t instruction, uint64_t& size);

    void
    writeByteAtAddress(uint8_t instruction, size_t offset);

    void
    writeWord(uint16_t instruction);

    uint32_t
    getDoubleWord(uint64_t offset);

    size_t
    writeString(const char* string);

    uint64_t
    writeString(const Utf8StringView& string);

    uint64_t
    writeUleb128(uint64_t value);

    static
    size_t
    getUleb128Size(size_t value);

    size_t
    writeReverseUleb128(uint64_t value);

    void
    writeSleb128(int64_t integer);

    void
    writeReverseSleb128(int64_t i);

    void
    writePadding(int64_t padding);

    List<uint8_t>*
    output;

    uint64_t*
    offset;
};


}
#endif //ELET_BYTEWRITER_H
