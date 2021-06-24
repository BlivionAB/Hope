#ifndef ELET_TEXTWRITER_H
#define ELET_TEXTWRITER_H


#include <Foundation/Utf8String.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>


namespace elet::domain::compiler::instruction
{


using namespace foundation;


class TextWriter
{

public:

    TextWriter(unsigned int indentationStep = 4);

    void
    addColumn(unsigned int spaces);

    void
    tab();

    void
    writeUint(unsigned int integer);

    void
    writeSize(std::size_t size);

    void
    writeCharacter(char character);

    void
    writeCString(const char* text);

    void
    writeZeroLength(const Utf8StringView& text);

    void
    write(const Utf8StringView& text);

    void
    writeLine(const Utf8StringView& text);

    void
    writeImmediateValue(uint64_t integer);

    void
    writeDisplacement(int32_t n);

    void
    writeByteHex(uint8_t integer);

    void
    write(char ch);

    void
    writeAddress64(uint64_t address);

    void
    writeByteWithHexPrefix(uint8_t integer);

    void
    writeSignedHex(std::array<uint8_t, 4> integer);

    void
    space();

    void
    newline();

    void
    indent();

    void
    unindent();

    void
    clearIndent();

    Utf8String&
    toString();

    void
    setStartSlice();

private:

    Utf8String
    _output;

    unsigned int
    _column = 0;

    unsigned int
    _indentation = 0;

    unsigned int
    _indentationStep;

    const char*
    _startOfSlice;

    List<unsigned int>
    _columns;

    void
    writeIndent();
};


}

#endif //ELET_TEXTWRITER_H
