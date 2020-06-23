#include "TextSpanChunkIterator.h"
#include "Foundation/Buffer.h"
#include <algorithm>

using namespace elet::foundation;

TextSpanChunkIterator::TextSpanChunkIterator(Buffer* buffer, std::size_t size, const char* value):
    buffer(buffer),
    sizeLeft(size),
    value(value)
{ }

TextSpanChunk
TextSpanChunkIterator::operator *() const
{
    return TextSpanChunk {
        value,
        std::min(sizeLeft, buffer->size),
    };
}

bool
TextSpanChunkIterator::operator ++ ()
{
    sizeLeft -= buffer->size;
    buffer = buffer->next;
    if (buffer == nullptr)
    {
        return false;
    }
    value = buffer->value;
    return true;
}

bool
TextSpanChunkIterator::operator == (const TextSpanChunkIterator& other) const
{
    return value == other.value;
}

bool
TextSpanChunkIterator::operator != (const TextSpanChunkIterator& other) const
{
    return sizeLeft != 0;
}