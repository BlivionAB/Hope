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
        if (column == _column)
        {
            continue;
        }
        else if (column >= _column)
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
TextWriter::write(char ch)
{
    _output += ch;
    _column += 1;
}


void
TextWriter::writeZeroLength(const Utf8StringView& text)
{
    _output += text;
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
    auto old = _output.size();
    _output += std::to_string(digits).c_str();
    _column += _output.size() - old;
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


void
TextWriter::newline()
{
    _output += "\n";
    _column = 0;
    writeIndent();
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
    writeByteHex(integer);
}


void
TextWriter::writeDisplacement(int32_t n)
{
    if (n & 0x80)
    {
        write("-");
        n = ~n + 1;
    }
    write("0x");
    static const char* digits = "0123456789abcdef";
    bool hasWrittenDigit = false;
    for (int i = 7; i >= 0; --i)
    {
        unsigned int s = std::pow(16, i);
        unsigned int r = n / s;
        if (r != 0 || hasWrittenDigit)
        {
            hasWrittenDigit = true;
            write(digits[r]);
            if (r != 0)
            {
                n -= r * s;
            }
        }
    }
    if (!hasWrittenDigit)
    {
        write("0");
    }
}


void
TextWriter::writeImmediateValue(uint64_t integer)
{
    write("$");
    writeDisplacement(integer);
}


void
TextWriter::writeSignedHex(std::array<uint8_t, 4> integer)
{
    bool isSigned = (integer[3] & 0b10000000);
    if (isSigned)
    {
        write("-");
    }
    uint32_t n = 0;

    for (int i = 0; i < 4; ++i)
    {
        if (isSigned)
        {
            n += ((~integer[i] + 1) << (i * 8));
        }
        else
        {
            n += (integer[i] << (i * 8));
        }
    }
    writeDisplacement(n);
}


void
TextWriter::writeByteHex(uint8_t integer)
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

void
TextWriter::writeAddress64(uint64_t address)
{
    write("0x");
    writeByteHex((address >> 56) & 0xff);
    writeByteHex((address >> 48) & 0xff);
    writeByteHex((address >> 40) & 0xff);
    writeByteHex((address >> 32) & 0xff);
    writeByteHex((address >> 24) & 0xff);
    writeByteHex((address >> 16) & 0xff);
    writeByteHex((address >> 8) & 0xff);
    writeByteHex(address & 0xff);
}

void
TextWriter::writeCString(const char* text)
{
    _output += text;
    _column += std::strlen(text);
}



void
TextWriter::write(int64_t integer)
{
    int exp = 64;
    int rest = integer;
    bool hasSeenNonZeroInt = false;
    while (exp >= 0)
    {
        int base = pow(10, exp);
        int result = rest / base;
        if (result != 0 || hasSeenNonZeroInt)
        {
            write(std::to_string(result).c_str());
            hasSeenNonZeroInt = true;
        }
        rest = rest % base;
        --exp;
    }
}


void
TextWriter::write(uint64_t integer)
{
    int exp = 64;
    int rest = integer;
    bool hasSeenNonZeroInt = false;
    while (exp >= 0)
    {
        int base = pow(10, exp);
        int result = rest / base;
        if (result != 0 || hasSeenNonZeroInt)
        {
            write(std::to_string(result).c_str());
            hasSeenNonZeroInt = true;
        }
        rest = rest % base;
        --exp;
    }
}

}