#ifndef FLASHPOINT_FIRST_CHARACTER_HASH_H
#define FLASHPOINT_FIRST_CHARACTER_HASH_H

#include <cstddef>
#include "Foundation/Memory/Utf8Span.h"
#include "Foundation/Utf8StringView.h"

template <typename T>
struct FirstCharacterHash
{
    std::size_t
    operator() (const Utf8Span& value, std::size_t tableSize) const;

    std::size_t
    operator() (const Utf8Span* value, std::size_t tableSize) const;

    std::size_t
    operator() (const Utf8String& value, std::size_t tableSize) const;

    std::size_t
    operator() (const Utf8StringView& value, std::size_t tableSize) const;

    std::size_t
    operator() (const char* value, std::size_t tableSize) const;

    std::size_t
    operator() (std::size_t value, std::size_t tableSize) const;
};

#include "FirstCharacterHashImpl.h"

#endif //FLASHPOINT_FIRST_CHARACTER_HASH_H
