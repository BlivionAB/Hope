#ifndef FLASHPOINT_TOKEN_VALUE_H
#define FLASHPOINT_TOKEN_VALUE_H

#include "Foundation/Memory/DataStructure/MemoryPooledVector.h"
#include "Foundation/Buffer.h"
#include "TextSpanCharacterIterator.h"
#include "TextSpanChunkIterator.h"
#include <vector>
#include <string>

using namespace elet::foundation;

struct TextSpan
{
    const char*
    value;

    std::size_t
    size;

    Buffer*
    buffer;

    void*
    operator
    new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    void*
    operator
    new(std::size_t size);

    TextSpan(const char* value, std::size_t length);

    TextSpan(const char* value, std::size_t length, struct Buffer* buffer);

    bool
    operator
    == (const TextSpan* other) const;

    bool
    operator
    == (const char* other) const;

    bool
    operator
    != (const TextSpan* other) const;

    bool
    operator
    != (const char* other) const;

    char
    operator
    [] (std::size_t currentBufferIndex) const;

    char*
    toCharArray();

    TextSpanCharacterIterator
    begin() const;

    TextSpanCharacterIterator
    end() const;

    TextSpanChunkIterator
    getChunkIterator() const;

    bool
    IsEqual(TextSpan* other);

    bool
    isEqual(const char *other);
};

struct TextSpanComparer
{
    bool operator()(const TextSpan* a, const TextSpan* b) const;
};

char*
ToCharArray(const TextSpan* textSpan);

char*
ToCharArray(const std::vector<TextSpan*>& token_value_list);

#endif //FLASHPOINT_TOKEN_VALUE_H
