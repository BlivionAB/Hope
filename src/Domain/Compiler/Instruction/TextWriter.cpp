#include "TextWriter.h"
#include <array>
#include <cmath>


namespace elet::domain::compiler::instruction
{


TextWriter::TextWriter(unsigned int indentationStep):
    _indentationStep(indentationStep)
{

}


void
TextWriter::addColumn(unsigned int spaces)
{
    _columns.add(spaces);
}


void
TextWriter::tab()
{
    for (const auto& column : _columns)
    {
        if (column >= _column)
        {
            unsigned int diff = column - _column;
            for (unsigned int i = 0; i < diff; ++i)
            {
                space();
            }
            break;
        }
    }
}


void
TextWriter::write(const Utf8StringView& text)
{
    _output += text;
    _column += text.size();
}


void
TextWriter::writeUint(unsigned int digits)
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
    _column += 1;
}


void
TextWriter::writeLine(const Utf8StringView &text)
{
    write(text);
    newline();
    _column += text.size() + 1;
    writeIndent();
}


void TextWriter::newline()
{
    _output += "\n";
    _column = 0;
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
            _column += 1;
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
    _column += 1;
}

void
TextWriter::writeByteWithHexPrefix(uint8_t integer)
{
    write("0x");
    writeByte(integer);
}


void
TextWriter::writeByte(uint8_t integer)
{
    int exp = 1;
    int rest = integer;
    while (exp >= 0)
    {
        int base = pow(16, exp);
        int result = rest / base;
        switch (result)
        {
            case 10:
                write("a");
                break;
            case 11:
                write("b");
                break;
            case 12:
                write("c");
                break;
            case 13:
                write("d");
                break;
            case 14:
                write("e");
                break;
            case 15:
                write("f");
                break;
            default:
                write(std::to_string(result).c_str());
                break;
        }
        rest = rest % base;
        --exp;
    }
}


}