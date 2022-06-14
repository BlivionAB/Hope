#include "TextWriter.h"
#include <array>
#include <cmath>


namespace elet::foundation
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
    TextWriter::write(Utf8String::Character ch)
    {
        _output += static_cast<char>(ch);
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
        if (_column < _indentation * _indentationStep)
        {
            writeIndent();
        }
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
    }


    void
    TextWriter::newline()
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
    TextWriter::writeSignedHexValue(int64_t n)
    {
        if (n < 0)
        {
            write("-");
            n = ~n + 1;
        }
        writeU64HexValue(n);
    }


    // TODO: write test writeU32HexValue
    void
    TextWriter::writeU32HexValue(uint64_t n)
    {
        write("0x");
        n = n & 0x00000000'ffffffff;
        static const char* digits = "0123456789abcdef";
        bool hasWrittenDigit = false;
        int writeDigitsCount = 0;
        for (int i = 8; i >= 0; --i)
        {
            uint64_t s = pow(16, i);
            uint64_t r = n / s;
            if (r != 0 || hasWrittenDigit)
            {
                hasWrittenDigit = true;
                if (writeDigitsCount != 0 && writeDigitsCount % 4 == 0)
                {
                    write("_");
                }
                write(digits[r]);
                writeDigitsCount++;
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
    TextWriter::writeU64HexValue(uint64_t n)
    {
        write("0x");
        static const char* digits = "0123456789abcdef";
        bool hasWrittenDigit = false;
        int writeDigitsCount = 0;
        for (int i = 15; i >= 0; --i)
        {
            uint64_t s = pow(16, i);
            uint64_t r = n / s;
            if (r != 0 || hasWrittenDigit)
            {
                hasWrittenDigit = true;
                if (writeDigitsCount != 0 && writeDigitsCount % 4 == 0)
                {
                    write("_");
                }
                write(digits[r]);
                writeDigitsCount++;
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
    TextWriter::writeSignedImmediateValue(uint64_t integer)
    {
        write("#");
        writeSignedHexValue(integer);
    }


    void
    TextWriter::writeSignedHexValue(std::array<uint8_t, 4> integer)
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
        writeSignedHexValue(n);
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
    TextWriter::write(int32_t integer)
    {
        if (integer < 0)
        {
            write("-");
            writeDecimal(static_cast<uint64_t>(integer * -1));
        }
        else
        {
            writeDecimal(static_cast<uint64_t>(integer));
        }
    }


    void
    TextWriter::write(int64_t integer)
    {
        if (integer < 0)
        {
            write("-");
            writeDecimal(static_cast<uint64_t>(integer * -1));
        }
        else
        {
            writeDecimal(static_cast<uint64_t>(integer));
        }
    }


    void
    TextWriter::write(const Int128& integer)
    {
        if (integer < 0)
        {
            write("-");
            writeDecimal(integer * Int128(-1));
        }
        else
        {
            writeDecimal(integer);
        }
    }


    void
    TextWriter::write(uint64_t integer)
    {
        writeDecimal(integer);
    }


    void
    TextWriter::writeDecimal(Int128 integer)
    {
        Int128 rest = integer;
        bool hasSeenNonZeroInt = false;
        int baseSize = 19;
        while (baseSize >= 0)
        {
            Int128 base = Int128::pow(10, baseSize);
            Int128 result = rest / base;
            if (result != Int128(0) || hasSeenNonZeroInt)
            {
                writeCharacter(static_cast<uint32_t>(result) + '0');
                hasSeenNonZeroInt = true;
            }
            rest = rest % base;
            --baseSize;
        }
        if (!hasSeenNonZeroInt)
        {
            write("0");
        }
    }


    void
    TextWriter::writeIndentedText(const Utf8String& text)
    {
        bool firstLine = true;
        bool trackIndentation = true;
        int currentIndentation = 0;
        for (const Utf8String::Character ch : text)
        {
            if (ch == Utf8String::Character::Newline)
            {
                trackIndentation = true;
                newline();
                continue;
            }
            if (trackIndentation && ch == Utf8String::Character::Space)
            {
                currentIndentation++;
                continue;
            }
            if (trackIndentation && !firstLine)
            {
                writeIndent();
            }
            for (int i = 0; i < currentIndentation; i++)
            {
                space();
            }
            currentIndentation = 0;
            trackIndentation = false;
            firstLine = false;
            write(ch);
        }
    }
}