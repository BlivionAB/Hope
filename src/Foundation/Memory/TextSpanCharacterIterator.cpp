#include "TextSpanCharacterIterator.h"
#include "Foundation/Buffer.h"

using namespace elet::foundation;

TextSpanCharacterIterator::TextSpanCharacterIterator(Buffer* buffer, std::size_t size, std::size_t bufferIndex):
    buffer(buffer),
    size(size),
    bufferIndex(bufferIndex),
    value(buffer->value[bufferIndex])
{ }

char
TextSpanCharacterIterator::operator*() const
{
    return value;
}

const TextSpanCharacterIterator&
TextSpanCharacterIterator::operator++()
{
    bufferIndex++;
    index++;
    if (index >= size)
    {
        value = '\0';
        return *this;
    }
    if (bufferIndex >= buffer->size)
    {
        buffer = buffer->next;
        bufferIndex = 0;
    }
    value = buffer->value[bufferIndex];
    return *this;
}

bool
TextSpanCharacterIterator::operator == (const TextSpanCharacterIterator& other) const
{
    return value == other.value;
}

bool
TextSpanCharacterIterator::operator != (const TextSpanCharacterIterator& other) const
{
    return value != other.value;
}