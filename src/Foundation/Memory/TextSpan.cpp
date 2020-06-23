#include "Foundation/Memory/DataStructure/MemoryPooledVector.h"
#include "Foundation/Memory/TextSpan.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace elet::foundation;

void*
Buffer::operator new(std::size_t size)
{
    return malloc(size);
}

void*
Buffer::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    return memoryPool->allocate(size, alignof(Buffer), ticket);
}

void*
TextSpan::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    return memoryPool->allocate(size, alignof(TextSpan), ticket);
}

void*
TextSpan::operator new(std::size_t size)
{
    return malloc(size);
}

TextSpan::TextSpan(const char* value, std::size_t length):
    value(value),
    size(length)
{ }

TextSpan::TextSpan(const char* value, std::size_t length, struct Buffer* buffer):
    value(value),
    size(length),
    buffer(buffer)
{ }

bool
TextSpan::operator == (const TextSpan* other) const
{
    if (size != other->size)
    {
        return false;
    }
    if (value == other->value)
    {
        return true;
    }
    // TODO: Optimize this
    for (std::size_t i = 0; i < size; i++)
    {
        if ((*this)[i] != (*other)[i])
        {
            return false;
        }
    }
    return true;
}

bool
TextSpan::operator == (const char* other) const
{
    std::size_t i = 0;
    while (true)
    {
        if (i == size)
        {
            return other[i] == '\0';
        }
        if (other[i] == '\0')
        {
            return false;
        }
        if ((*this)[i] != other[i])
        {
            return false;
        }
        i++;
    }
}

bool
TextSpan::operator !=(const TextSpan* other) const
{
    return !(*this == other);
}

bool
TextSpan::operator !=(const char* other) const
{
    return !(*this == other);
}

char
TextSpan::operator[](std::size_t index) const
{
    Buffer* currentBuffer = buffer;
    std::size_t startIndex = (value - currentBuffer->value) / sizeof(char);
    std::size_t currentBufferIndex = index + startIndex;
    std::size_t bufferLengthRemaining = size + startIndex;
    while (true)
    {
        if (currentBufferIndex < currentBuffer->size)
        {
            return currentBuffer->value[currentBufferIndex];
        }
        else if (currentBufferIndex < bufferLengthRemaining)
        {
            currentBufferIndex -= currentBuffer->size;
            bufferLengthRemaining -= currentBuffer->size;
            currentBuffer = currentBuffer->next;
        }
        else {
            throw std::logic_error("Could not find text span at position.");
        }
    }
}

TextSpanCharacterIterator
TextSpan::begin() const
{
    return TextSpanCharacterIterator(buffer, size, this->value - this->buffer->value);
}

TextSpanCharacterIterator
TextSpan::end() const
{
    TextSpanCharacterIterator iterator(buffer, size, 0);
    iterator.value = '\0';
    return iterator;
}

TextSpanChunkIterator
TextSpan::getChunkIterator() const
{
    return TextSpanChunkIterator(buffer, size, value);
}

bool
TextSpan::IsEqual(TextSpan* other)
{
    return value == other->value;
}

bool
TextSpan::isEqual(const char *other)
{
    std::size_t n = 0;
    std::size_t otherLength = strlen(other);
    if (size != otherLength)
    {
        return false;
    }
    while (n < size)
    {
        if ((*this)[n] != other[n]) {
            return false;
        }
        n++;
    }
    return true;
}

bool
TextSpanComparer::operator ()(const TextSpan* a, const TextSpan* b) const
{
    auto min = std::min(a->size, b->size);
    for (std::size_t i = 0; i < min; i++) {
        if (a->value[i] > b->value[i]) {
            return true;
        }
        else if (a->value[i] < b->value[i]) {
            return false;
        }
        else {
            continue;
        }
    }
    return a->size > b->size;
};

char*
TextSpan::toCharArray()
{
    char* str = new char[this->size + 1];
    std::size_t sizeLeft = this->size;
    Buffer* currentBuffer = this->buffer;
    const char* cursor = this->value;

    std::size_t startSize = this->value - currentBuffer->value;
    std::size_t firstBufferSize = std::min(sizeLeft, currentBuffer->size - startSize);
    std::strncat(str, cursor, firstBufferSize);
    sizeLeft -= firstBufferSize;
    while (sizeLeft > 0)
    {
        currentBuffer = currentBuffer->next;
        cursor = currentBuffer->value;
        if (sizeLeft < currentBuffer->size)
        {
            std::strncat(str, cursor, sizeLeft);
            break;
        }
        std::strncat(str, cursor, currentBuffer->size);
        sizeLeft -= currentBuffer->size;
    }
    return str;
}
