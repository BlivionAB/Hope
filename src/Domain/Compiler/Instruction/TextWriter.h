#ifndef ELET_TEXTWRITER_H
#define ELET_TEXTWRITER_H


#include <Foundation/Utf8String.h>
#include <Foundation/Utf8StringView.h>


namespace elet::domain::compiler::instruction
{


using namespace foundation;


class TextWriter
{

public:

    TextWriter(unsigned int indentationStep = 4);

    void
    writeUnsignedInteger(unsigned int integer);

    void
    writeSize(std::size_t size);

    void
    writeCharacter(char character);

    void
    write(const Utf8StringView& text);

    void
    writeLine(const Utf8StringView& text);

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
    _indentation = 0;

    unsigned int
    _indentationStep;

    const char*
    _startOfSlice;

    void
    writeIndent();
};


}

#endif //ELET_TEXTWRITER_H
