#include "TextWriter.h"

namespace elet::domain::compiler::instruction
{


TextWriter::TextWriter(unsigned int indentationStep):
    _indentationStep(indentationStep)
{

}


void
TextWriter::write(const Utf8StringView& text)
{
    _output += text;
}


void
TextWriter::writeUnsignedInteger(unsigned int digits)
{
    _output += std::to_string(digits).c_str();
}


void
TextWriter::writeSize(std::size_t size)
{
    _output += std::to_string(size).c_str();
}


void
TextWriter::writeCharacter(char character)
{
    _output += character;
}


void
TextWriter::writeLine(const Utf8StringView &text)
{
    write(text);
    newline();
    writeIndent();
}


void TextWriter::newline()
{
    _output += "\n";
}


Utf8String&
TextWriter::toString()
{
    return _output;
}


void
TextWriter::indent()
{
    _indentation++;
    writeIndent();
}


void
TextWriter::writeIndent()
{
    for (unsigned int i = 0; i < _indentation; i++)
    {
        for (unsigned int i = 0; i < _indentationStep; i++)
        {
            _output += " ";
        }
    }
}


void
TextWriter::unindent()
{
    _indentation--;
}

void
TextWriter::clearIndent()
{
    _indentation = 0;
}


void
TextWriter::space()
{
    _output += " ";
}

void
TextWriter::writeDoubleWordHexInteger(std::uint32_t integer)
{

}


}