#ifndef ELET_BYTEWRITER_H
#define ELET_BYTEWRITER_H


#include <cstdint>
#include <Foundation/List.h>


namespace elet::domain::compiler::instruction::output
{


using namespace elet::foundation;


class ByteWriter
{

public:

    ByteWriter(List<uint8_t>* output, size_t* offset);

    void
    writeQuadWord(std::uint64_t instruction);


    void
    writeDoubleWord(std::uint32_t instruction);


    void
    writeDoubleWordAtAddress(std::uint32_t instruction, std::size_t offset);

    void
    writeByte(std::uint8_t instruction);

    void
    writeString(const char* string);

private:

    List<uint8_t>*
    _output;

    size_t*
    _offset;

};


}
#endif //ELET_BYTEWRITER_H
