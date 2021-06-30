#ifndef ELET_BYTEWRITER_H
#define ELET_BYTEWRITER_H


#include <cstdint>
#include <Foundation/List.h>
#include <Foundation/Utf8StringView.h>


namespace elet::domain::compiler::instruction::output
{


using namespace elet::foundation;


class ByteWriter
{

public:

    ByteWriter(List<uint8_t>* output, uint64_t* offset);

    void
    writeQuadWord(std::uint64_t instruction);

    void
    writeQuadWordAtAddress(std::uint64_t instruction, std::uint64_t);


    void
    writeDoubleWord(std::uint32_t instruction);


    void
    writeDoubleWordAtAddress(std::uint32_t instruction, std::size_t offset);

    void
    writeByte(std::uint8_t instruction);

    void
    writeByte(std::uint8_t instruction, uint64_t& size);

    void
    writeByteAtAddress(std::uint8_t instruction, std::size_t offset);

    void
    writeWord(uint16_t instruction);

    uint32_t
    getDoubleWord(uint64_t offset);

    size_t
    writeString(const char* string);

    void
    writeString(const Utf8StringView& string);

    void
    writeUleb128(uint64_t integer);

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
