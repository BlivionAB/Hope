#ifndef FLASHPOINT_TEXT_SPAN_CHARACTER_ITERATOR_H
#define FLASHPOINT_TEXT_SPAN_CHARACTER_ITERATOR_H

#include "Foundation/Buffer.h"

using namespace elet::foundation;

class TextSpanCharacterIterator
{
public:

    friend class TextSpan;

    char
    operator
    *() const;

    const TextSpanCharacterIterator&
    operator
    ++();

    bool
    operator
    ==(const TextSpanCharacterIterator &other) const;

    bool
    operator
    !=(const TextSpanCharacterIterator &other) const;

    TextSpanCharacterIterator(Buffer* buffer, std::size_t size, std::size_t bufferIndex);

private:

    Buffer*
    buffer;

    std::size_t
    index = 0;

    std::size_t
    bufferIndex;

    std::size_t
    size;

    char
    value;
};


#endif //FLASHPOINT_TEXT_SPAN_CHARACTER_ITERATOR_H
