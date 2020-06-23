#ifndef FLASHPOINT_UTF8SPAN_H
#define FLASHPOINT_UTF8SPAN_H

#include "Foundation/Memory/DataStructure/MemoryPooledVector.h"
#include "Foundation/Memory/TextSpan.h"
#include "Foundation/Utf8String.h"
#include "Utf8SpanIterator.h"

#define UTF8_EOF 0x04

using namespace elet::foundation;

class Utf8Span
{
public:

    Utf8Span(TextSpan* source);

    Utf8Span(TextSpan* source, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    // Only used fo default init. That should later be replaced.
    Utf8Span();

    Utf8Span
    subString(std::size_t start, std::size_t end) const;

    Utf8Span
    subString(std::size_t start) const;

    char*
    toCharArray() const;

    bool
    isEqualFrom(const char* text, std::size_t fromIndex) const;

    operator TextSpan*();

    Utf8SpanIterator
    begin() const;

    Utf8SpanIterator
    end() const;

    std::size_t
    size() const;

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    char
    operator [] (std::size_t memoryIndex) const;

    bool
    operator == (const Utf8Span& other) const;

    bool
    operator == (const Utf8String& other) const;

    bool
    operator != (const Utf8Span& other) const;

    bool
    operator != (const Utf8String& other) const;

    TextSpan*
    source;

    Utf8String
    toString() const;

private:

    MemoryPool*
    memoryPool;

    MemoryPoolTicket*
    ticket;
};

#endif //FLASHPOINT_UTF8SPAN_H
