#include "Utf8String.h"
#include <cstddef>
#include <ostream>
#include <string>


Utf8String::Utf8String():
    _capacity(0)
{
    _value = new char[1];
    _value[0] = '\0';
    _end = _value;
}

Utf8String::Utf8String(const char* text)
{
    std::size_t size = std::strlen(text);
    _value = new char[size + 1];
    _end = _value + size;
    std::strncpy(_value, text, size + 1);
    _capacity = size;
}

Utf8String::Utf8String(const char* text, std::size_t size):
    _capacity(size)
{
    _value = new char[size + 1];
    std::strncpy(_value, text, size);
    _value[size] = '\0';
    _end = _value + size;
}

Utf8String::Utf8String(const Utf8String& copy)
{
    std::size_t size = copy.size();
    _value = new char[size + 1];
    std::strncpy(_value, copy._value, size);
    _value[size] = '\0';
    _end = _value + size;
    _capacity = copy._capacity;
}


Utf8String::~Utf8String()
{
    delete[] _value;
}

void
Utf8String::reserve(std::size_t size)
{
    _capacity = size;
}

bool
Utf8String::operator != (const Utf8String& target) const
{
    return !(*this == target);
}

bool
Utf8String::operator == (const Utf8String& target) const
{
    if (size() != target.size())
    {
        return false;
    }
    return std::strncmp(_value, target._value, size()) == 0;
}

bool
Utf8String::operator == (const char* target) const
{
    return std::strncmp(_value, target, size()) == 0;
}

void
Utf8String::operator += (const Utf8String& other)
{
    std::size_t sizeOfNewText = other.size();
    std::size_t oldSize = size();
    std::size_t newSize = oldSize + sizeOfNewText;
    if (newSize > _capacity)
    {
        _value = reinterpret_cast<char*>(realloc(_value, newSize + 1));
        std::strncpy(&_value[oldSize], other._value, sizeOfNewText + 1);
        _capacity = newSize;
    }
    else
    {
        std::strncpy(_end, other._value, sizeOfNewText + 1);
    }
    _end = &_value[newSize];
}


void
Utf8String::operator += (const char ch)
{
    std::size_t oldSize = size();
    std::size_t newSize = oldSize + 1;
    if (newSize > _capacity)
    {
        _value = reinterpret_cast<char*>(realloc(_value, newSize + 1));
        _value[oldSize] = ch;
        _capacity = newSize;
    }
    else
    {
        _value[oldSize] = ch;
    }
    _end = &_value[newSize];
}

void
Utf8String::operator += (const char* text)
{
    std::size_t sizeOfNewText = strlen(text);
    std::size_t oldSize = size();
    std::size_t newSize = oldSize + sizeOfNewText;
    if (newSize > _capacity)
    {
        _value = reinterpret_cast<char*>(realloc(_value, newSize + 1));
        std::strncpy(&_value[oldSize], text, sizeOfNewText + 1);
        _capacity = newSize;
    }
    else
    {
        std::strncpy(_end, text, sizeOfNewText + 1);
    }
    _end = &_value[newSize];
}

bool
Utf8String::operator < (const Utf8String& text) const
{
    return std::strcmp(_value, text._value) < 0;
}

Utf8String
Utf8String::operator + (const Utf8String& string) const
{
    Utf8String result(*this);
    result += string;
    return result;
}

Utf8String&
Utf8String::operator = (const Utf8String& other)
{
    if (_capacity < other._capacity)
    {
        _value = reinterpret_cast<char*>(realloc(_value, other._capacity));
        _capacity = other._capacity;
    }
    strncpy(_value, other._value, other.size());
    _end = &_value[other.size()];
    return *this;
}

const char*
Utf8String::toString() const
{
    std::size_t _size = size();
    char* result = new char[_size + 1];
    std::strncpy(result, _value, _size + 1);
    return result;
}


const char*
Utf8String::asString() const
{
    return _value;
}

void
Utf8String::set(const char* text, std::size_t size)
{
    _value = reinterpret_cast<char*>(realloc(_value, size + 1));
    std::strncpy(_value, text, size + 1);
    _capacity = size;
    _end = &_value[size];
}

void
Utf8String::append(const char* text, std::size_t size)
{
    std::size_t oldSize = this->size();
    std::size_t newSize = oldSize + size;
    if (newSize > _capacity)
    {
        _value = reinterpret_cast<char*>(realloc(_value, newSize + 1));
        std::strncpy(&_value[oldSize], text, size + 1);
        _capacity = newSize;
    }
    else
    {
        std::strncpy(_end, text, size + 1);
    }
    _end = &_value[newSize];
}

std::ostream&
operator << (std::ostream& out, const Utf8String& string)
{
    out << std::string(string.toString());
    return out;
}

char
Utf8String::operator [] (std::size_t index) const
{
    return _value[index];
}

Utf8String::Iterator
Utf8String::begin() const
{
    return Iterator(_value);
}

Utf8String::Iterator
Utf8String::end() const
{
    return Iterator(_end);
}

Utf8String
operator + (const char* string1, const Utf8String& string2)
{
    Utf8String result(string1);
    return result + string2;
}

std::size_t
Utf8String::size() const
{
    return _end - _value;
}

const unsigned char kFirstBitMask = 128; // 1000000
const unsigned char kSecondBitMask = 64; // 0100000
const unsigned char kThirdBitMask = 32;  // 0010000
const unsigned char kFourthBitMask = 16; // 0001000
const unsigned char kFifthBitMask = 8;   // 0000100

Utf8String::Iterator::Iterator(const char* value):
    _value(value)
{

}

const char*
Utf8String::Iterator::getPosition() const
{
    return _value;
}

void
Utf8String::Iterator::operator++ ()
{
    unsigned char ch = *_value;
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
    _value += offset;
}

void
Utf8String::Iterator::operator += (std::size_t offset)
{
    while (offset != 0)
    {
        ++(*this);
        offset--;
    }
}

bool
Utf8String::Iterator::operator == (const Utf8String::Iterator& other) const
{
    return _value == other._value;
}


bool
Utf8String::Iterator::operator != (const Utf8String::Iterator& other) const
{
    return _value != other._value;
}

Utf8String::Character
Utf8String::Iterator::operator * () const
{
    int codePoint = 0;
    unsigned char firstByte = *_value;

    if (firstByte & kFirstBitMask)
    {
        if (firstByte & kThirdBitMask)
        {
            if (firstByte & kFourthBitMask)
            {
                codePoint = (firstByte & 0x07u) << 18u;
                unsigned char secondByte = *(_value + 1);
                codePoint += (secondByte & 0x3fu) << 12u;
                unsigned char thirdByte = *(_value + 2);
                codePoint += (thirdByte & 0x3fu) << 6u;;
                unsigned char fourthByte = *(_value + 3);
                codePoint += (fourthByte & 0x3fu);
            }
            else
            {
                codePoint = (firstByte & 0x0fu) << 12u;
                unsigned char secondByte = *(_value + 1);
                codePoint += (secondByte & 0x3fu) << 6u;
                unsigned char thirdByte = *(_value + 2);
                codePoint += (thirdByte & 0x3fu);
            }
        }
        else
        {
            codePoint = (firstByte & 0x1fu) << 6u;
            unsigned char secondByte = *(_value + 1);
            codePoint += (secondByte & 0x3fu);
        }
    }
    else
    {
        codePoint = firstByte;
    }
    return static_cast<Character>(codePoint);
}
