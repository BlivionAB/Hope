#include "Utf8StringView.h"

namespace elet::foundation
{

Utf8StringView::Utf8StringView()
{

}

Utf8StringView::Utf8StringView(const Utf8String& string)
{
    const char* value = string.toString();
    _value = value;
    _end = &value[string.size()];
}

Utf8StringView::Utf8StringView(const char* value, const char* end):
    _value(value),
    _end(end)
{

}

Utf8StringView::Utf8StringView(const char* value):
    _value(value),
    _end(&value[std::strlen(value)])
{

}

Utf8StringView::Iterator
Utf8StringView::begin() const
{
    return Iterator(_value);
}

Utf8StringView::Iterator
Utf8StringView::end() const
{
    return Iterator(_end);
}

const char*
Utf8StringView::source() const
{
    return _value;
}


const char*
Utf8StringView::toString() const
{
    std::size_t size = _end - _value;
    char* result = new char[size + 1];
    std::strncpy(result, _value, size);
    result[size] = '\0';
    return result;
}


Utf8StringView
Utf8StringView::slice(const char* start) const
{
    return Utf8StringView(start, _end);
}


Utf8StringView
Utf8StringView::slice(const char* start, const char* end) const
{
    return Utf8StringView(start, end);
}


std::size_t
Utf8StringView::size() const
{
    return _end - _value;
}


char
Utf8StringView::operator[](std::size_t memoryIndex) const
{
    return _value[memoryIndex];
}


bool
Utf8StringView::operator == (const char* text) const
{
    if (size() != strlen(text))
    {
        return false;
    }
    return std::strncmp(_value, text, size()) == 0;
}


bool
Utf8StringView::operator == (const Utf8StringView& other) const
{
    return *this == other.toString();
}


bool
Utf8StringView::operator < (const Utf8StringView &other) const
{
    return std::strncmp(_value, other._value, size()) < 0;
}


Utf8StringView::Iterator::Iterator(const char* value):
    value(value)
{

}


const char*
Utf8StringView::Iterator::getPosition() const
{
    return value;
}


void
Utf8StringView::Iterator::setPosition(const char* position)
{
    value = position;
}

Utf8String::Character
Utf8StringView::Iterator::peekNextChar()
{
    const char* currentPosition = getPosition();
    ++(*this);
    Utf8String::Character _char = *(*this);
    value = currentPosition;
    return _char;
}


const unsigned char kFirstBitMask = 128; // 1000000
const unsigned char kSecondBitMask = 64; // 0100000
const unsigned char kThirdBitMask = 32;  // 0010000
const unsigned char kFourthBitMask = 16; // 0001000
const unsigned char kFifthBitMask = 8;   // 0000100

void
Utf8StringView::Iterator::operator++ ()
{
    unsigned char ch = *value;
    std::size_t offset = 1;
    if (ch & kFirstBitMask)
    {
        if (ch & kThirdBitMask)
        {
            if (ch & kFourthBitMask)
            {
                offset = 4;
            }
            else
            {
                offset = 3;
            }
        }
        else
        {
            offset = 2;
        }
    }
    value += offset;
}

void
Utf8StringView::Iterator::operator += (std::size_t offset)
{
    while (offset != 0)
    {
        ++(*this);
        offset--;
    }
}

bool
Utf8StringView::Iterator::operator == (const Utf8StringView::Iterator& other) const
{
    return value == other.value;
}


bool
Utf8StringView::Iterator::operator != (const Utf8StringView::Iterator& other) const
{
    return value != other.value;
}

Utf8String::Character
Utf8StringView::Iterator::operator * () const
{
    int codePoint = 0;
    unsigned char firstByte = *value;

    if (firstByte & kFirstBitMask)
    {
        if (firstByte & kThirdBitMask)
        {
            if(firstByte & kFourthBitMask)
            {
                codePoint = (firstByte & 0x07u) << 18u;
                unsigned char secondByte = *(value + 1);
                codePoint += (secondByte & 0x3fu) << 12u;
                unsigned char thirdByte = *(value + 2);
                codePoint += (thirdByte & 0x3fu) << 6u;;
                unsigned char fourthByte = *(value + 3);
                codePoint += (fourthByte & 0x3fu);
            }
            else
            {
                codePoint = (firstByte & 0x0fu) << 12u;
                unsigned char secondByte = *(value + 1);
                codePoint += (secondByte & 0x3fu) << 6u;
                unsigned char thirdByte = *(value + 2);
                codePoint += (thirdByte & 0x3fu);
            }
        }
        else
        {
            codePoint = (firstByte & 0x1fu) << 6u;
            unsigned char secondByte = *(value + 1);
            codePoint += (secondByte & 0x3fu);
        }
    }
    else
    {
        codePoint = firstByte;
    }
    return static_cast<Utf8String::Character>(codePoint);
}

void
operator += (Utf8String& string1, const Utf8StringView& string2)
{
    string1 += string2.toString();
}

bool
operator == (const char* string1, const Utf8StringView& string2)
{
    return std::strncmp(string1, string2.source(), string2.size()) == 0;
}

}
