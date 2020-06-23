#ifndef FLASHPOINT_UTF8SPANITERATOR_H
#define FLASHPOINT_UTF8SPANITERATOR_H

#include "Foundation/Buffer.h"
#include "TextSpan.h"

namespace elet::foundation {

class Utf8SpanIterator
{
public:

    Utf8SpanIterator(TextSpan* source, std::size_t offset, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    void
    markStartOfSequence();

    TextSpan*
    getTextSpanFromSequence() const;

    TextSpan*
    getTextSpanFromSequence(std::size_t offset) const;

    const char*
    getCharArrayFromSequence() const;

    int
    peekNextToken();

    int
    peekNextToken(std::size_t offset);

    bool
    isEnd();

    void
    operator
    ++ ();

    void
    operator
    += (std::size_t increment);

    int
    operator
    * () const;

    std::size_t
    offset;

    TextSpan*
    source;

private:

    std::size_t
    bufferOffset;

    Buffer*
    currentBuffer;

    std::size_t
    currentBufferLength;

    Buffer*
    textSpanStartBuffer;

    std::size_t
    textSpanStartOffset;

    std::size_t
    textSpanStartBufferIndex;

    MemoryPool*
    memoryPool;

    MemoryPoolTicket*
    ticket;

    Buffer*
    savedCurrentBuffer;

    std::size_t
    savedBufferOffset;

    std::size_t
    savedOffset;

    char
    getCurrentCharacter() const;

    char
    getCurrentCharacter(int offset) const;

};

}

#endif //FLASHPOINT_UTF8SPANITERATOR_H
