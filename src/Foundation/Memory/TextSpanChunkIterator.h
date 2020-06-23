#ifndef FLASHPOINT_TEXT_SPAN_CHUNK_ITERATOR_H
#define FLASHPOINT_TEXT_SPAN_CHUNK_ITERATOR_H

#include "Foundation/Buffer.h"
#include <cstddef>

using namespace elet::foundation;

struct TextSpanChunk
{
    const char*
    value;

    std::size_t
    size;
};

class TextSpanChunkIterator
{
public:

    friend class TextSpan;

    TextSpanChunk
    operator
    *() const;

    bool
    operator
    ++();

    bool
    operator
    ==(const TextSpanChunkIterator &other) const;

    bool
    operator
    !=(const TextSpanChunkIterator &other) const;

    TextSpanChunkIterator(Buffer* buffer, std::size_t size, const char* value);

private:

    Buffer*
    buffer;

    std::size_t
    sizeLeft;

    std::size_t
    size;

    const char*
    value;
};


#endif //FLASHPOINT_TEXT_SPAN_CHARACTER_ITERATOR_H
