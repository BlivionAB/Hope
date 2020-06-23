#include "Utf8SpanIterator.h"

const unsigned char kFirstBitMask = 128; // 1000000
const unsigned char kSecondBitMask = 64; // 0100000
const unsigned char kThirdBitMask = 32;  // 0010000
const unsigned char kFourthBitMask = 16; // 0001000
const unsigned char kFifthBitMask = 8;   // 0000100

#define UTF8_EOF 0x04

using namespace elet::foundation;

Utf8SpanIterator::Utf8SpanIterator(TextSpan* source, std::size_t offset, MemoryPool* memoryPool, MemoryPoolTicket* ticket):
    source(source),
    currentBuffer(source->buffer),
    bufferOffset(source->value - source->buffer->value),
    offset(offset),
    memoryPool(memoryPool),
    ticket(ticket)
{ }

bool
Utf8SpanIterator::isEnd()
{
    return offset == source->size;
}

void
Utf8SpanIterator::operator += (std::size_t increment)
{
    for (std::size_t i = 0; i < increment; ++i)
    {
        ++(*this);
    }
}

void
Utf8SpanIterator::operator ++ ()
{
    unsigned char character = getCurrentCharacter();
    std::size_t _offset = 1;
    if (character & kFirstBitMask)
    {
        if (character & kThirdBitMask)
        {
            if (character & kFourthBitMask)
            {
                _offset = 4;
            }
            else
            {
                _offset = 3;
            }
        }
        else
        {
            _offset = 2;
        }
    }
    bufferOffset += _offset;
    offset += _offset;
    if (bufferOffset >= currentBuffer->size)
    {
        currentBuffer = currentBuffer->next;
        if (currentBuffer == nullptr) {
            return;
        }
        bufferOffset = 0;
    }
}

char
Utf8SpanIterator::getCurrentCharacter() const
{
    return currentBuffer->value[bufferOffset];
}

char
Utf8SpanIterator::getCurrentCharacter(int offset) const
{
    if (bufferOffset + offset >= currentBufferLength)
    {
        const Buffer* nextBuffer = currentBuffer->next;
        std::size_t i = bufferOffset + offset - currentBufferLength - 1;
        return nextBuffer->value[i];
    }
    return currentBuffer->value[bufferOffset + offset];
}

int
Utf8SpanIterator::operator *() const
{
    if (offset >= source->size)
    {
        return UTF8_EOF;
    }

    int codePoint = 0;
    unsigned char firstByte = getCurrentCharacter();

    if (firstByte & kFirstBitMask)
    {
        if (firstByte & kThirdBitMask)
        {
            if(firstByte & kFourthBitMask)
            {
                codePoint = (firstByte & 0x07u) << 18u;
                unsigned char secondByte = getCurrentCharacter(1);
                codePoint += (secondByte & 0x3fu) << 12u;
                unsigned char thirdByte = getCurrentCharacter(2);
                codePoint += (thirdByte & 0x3fu) << 6u;;
                unsigned char fourthByte = getCurrentCharacter(3);
                codePoint += (fourthByte & 0x3fu);
            }
            else
            {
                codePoint = (firstByte & 0x0fu) << 12u;
                unsigned char secondByte = getCurrentCharacter(1);
                codePoint += (secondByte & 0x3fu) << 6u;
                unsigned char thirdByte = getCurrentCharacter(2);
                codePoint += (thirdByte & 0x3fu);
            }
        }
        else
        {
            codePoint = (firstByte & 0x1fu) << 6u;
            unsigned char secondByte = getCurrentCharacter(1);
            codePoint += (secondByte & 0x3fu);
        }
    }
    else
    {
        codePoint = firstByte;
    }
    return codePoint;
}

int
Utf8SpanIterator::peekNextToken()
{
    return peekNextToken(1);
}

int
Utf8SpanIterator::peekNextToken(std::size_t offset)
{
    savedCurrentBuffer = currentBuffer;
    savedBufferOffset = bufferOffset;
    savedOffset = offset;
    for (std::size_t i = 0; i < offset; ++i)
    {
        ++(*this);
    }
    int ch = getCurrentCharacter();
    currentBuffer = savedCurrentBuffer;
    bufferOffset  = savedBufferOffset;
    offset = savedOffset;
    return ch;
}

void
Utf8SpanIterator::markStartOfSequence()
{
    textSpanStartBuffer = currentBuffer;
    textSpanStartOffset = offset;
    textSpanStartBufferIndex = bufferOffset;
}

TextSpan*
Utf8SpanIterator::getTextSpanFromSequence() const
{
    return new (memoryPool, ticket) TextSpan(&textSpanStartBuffer->value[textSpanStartBufferIndex], offset - textSpanStartOffset, textSpanStartBuffer);
}

TextSpan*
Utf8SpanIterator::getTextSpanFromSequence(std::size_t offset) const
{
    return new (memoryPool, ticket) TextSpan(&textSpanStartBuffer->value[textSpanStartBufferIndex], offset - textSpanStartOffset, textSpanStartBuffer);
}

const char*
Utf8SpanIterator::getCharArrayFromSequence() const
{
    TextSpan* sequence = getTextSpanFromSequence();
    return sequence->toCharArray();
}
