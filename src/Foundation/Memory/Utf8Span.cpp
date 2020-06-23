#include "Utf8Span.h"

Utf8Span::Utf8Span(TextSpan *source, MemoryPool* memoryPool, MemoryPoolTicket* ticket):
    source(source),
    memoryPool(memoryPool),
    ticket(ticket)
{ }

Utf8Span::Utf8Span(TextSpan* source):
    source(source)
{ }

Utf8Span::Utf8Span()
{ }

void*
Utf8Span::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    return memoryPool->allocate(size, alignof(Utf8Span *), ticket);
}

//Utf8Span::operator const char* ()
//{
//    return (char*)source;
//}
//
//Utf8Span::operator const TextSpan* ()
//{
//    return source;
//}

char*
Utf8Span::toCharArray() const
{
    return source->toCharArray();
}

bool
Utf8Span::isEqualFrom(const char* text, std::size_t fromIndex) const
{
    std::size_t accumulatedBufferSize = 0;
    std::size_t comparisonIndex = 0;
    Buffer* currentBuffer = source->buffer;
    std::size_t _size = size();
    do
    {
        if (accumulatedBufferSize + currentBuffer->size >= fromIndex)
        {
            do
            {
                std::size_t remaining = currentBuffer->size - (fromIndex - accumulatedBufferSize);
                if (std::strncmp(&currentBuffer->value[fromIndex - accumulatedBufferSize], &text[comparisonIndex], remaining) != 0)
                {
                    return false;
                }
                comparisonIndex = fromIndex - accumulatedBufferSize;
                fromIndex = 0;
                currentBuffer = currentBuffer->next;
                accumulatedBufferSize += currentBuffer->size;
            }
            while(accumulatedBufferSize >= _size);
        }
        currentBuffer = currentBuffer->next;
        accumulatedBufferSize += currentBuffer->size;
    }
    while (accumulatedBufferSize >= _size);
    return true;
}

Utf8Span
Utf8Span::subString(std::size_t start, std::size_t end) const
{
    std::size_t accumulatedBufferLength = 0;
    Buffer* currentBuffer = source->buffer;
    while (true)
    {
        if (accumulatedBufferLength + currentBuffer->size >= start)
        {
            TextSpan* source = new (memoryPool, ticket) TextSpan(&currentBuffer->value[start - accumulatedBufferLength], end - start, currentBuffer);
            return Utf8Span(source, memoryPool, ticket);
        }
        currentBuffer = currentBuffer->next;
        accumulatedBufferLength += currentBuffer->size;
    }
}

Utf8Span
Utf8Span::subString(std::size_t start) const
{
    return subString(start, size());
}

Utf8SpanIterator
Utf8Span::begin() const
{
    return Utf8SpanIterator(source, 0, memoryPool, ticket);
}

Utf8SpanIterator
Utf8Span::end() const
{
    return Utf8SpanIterator(source, source->size, memoryPool, ticket);
}

char
Utf8Span::operator [] (std::size_t memoryIndex) const
{
    std::size_t accumulatedBufferLength = 0;
    Buffer* currentBuffer = source->buffer;
    while (true)
    {
        if (accumulatedBufferLength + currentBuffer->size >= memoryIndex)
        {
            return currentBuffer->value[memoryIndex - accumulatedBufferLength];
        }
        currentBuffer = currentBuffer->next;
        accumulatedBufferLength += currentBuffer->size;
    }
}

bool
Utf8Span::operator == (const Utf8Span& other) const
{
    return (*source) == (other.source);
}

bool
Utf8Span::operator == (const Utf8String& other) const
{
    TextSpanChunkIterator iterator = source->getChunkIterator();
    const char* stringChars = other.toString();
    do
    {
        TextSpanChunk chunk = (*iterator);
        if (strncmp(chunk.value, stringChars, chunk.size) != 0)
        {
            return false;
        }
        ++iterator;
    }
    while(++iterator);
    return true;
}

bool
Utf8Span::operator != (const Utf8Span& other) const
{
    return !((*this) == other);
}

std::size_t
Utf8Span::size() const
{
    return source->size;
}

Utf8String
Utf8Span::toString() const
{
    return Utf8String(toCharArray());
}

Utf8Span::operator TextSpan*()
{
    return source;
}
