#ifndef FLASHPOINT_MEMORYPOOLEDPERFECTHASHTABLE_H
#define FLASHPOINT_MEMORYPOOLEDPERFECTHASHTABLE_H

#include "Foundation/Memory/TextSpan.h"
#include "Foundation/Memory/Utf8Span.h"
#include "Foundation/List.h"
#include <optional>
#include <set>

namespace apic::foundation::memory::datastructure
{

struct IntersectionChar {
    char
    character;

    List<const char*>*
    keys;

    IntersectionChar(char character, List<const char*>* keySignatures):
        character(character),
        keys(keySignatures)
    { }
};


class NonUniqueColumnKeyException : public std::runtime_error
{
public:
    NonUniqueColumnKeyException(std::string message) noexcept :
        std::runtime_error(message) { }
};

template<typename T>
struct KeyValue {
    const char* key;
    T value;
};

template<typename T>
class MemoryPooledPerfectHashTable
{
public:
    std::size_t
    capacity;

    std::size_t
    _size;

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    void*
    operator new(std::size_t size);

    MemoryPooledPerfectHashTable(std::initializer_list<std::pair<const char*, T>> list, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    MemoryPooledPerfectHashTable(std::initializer_list<std::pair<const char*, T>> list);

    MemoryPooledPerfectHashTable(MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    MemoryPooledPerfectHashTable(std::size_t capacity, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    MemoryPooledPerfectHashTable(std::size_t capacity);

    MemoryPooledPerfectHashTable();

    ~MemoryPooledPerfectHashTable();

    void
    insert(const char *key, const T &value);

    bool
    has(const TextSpan *key) const;

    bool
    has(const char *key) const;

    void
    index();

    std::optional<T>
    find(const char *key) const;

    std::optional<T>
    find(const TextSpan *key) const;

    std::size_t
    size() const;

    class Iterator {
        friend class HashTable;
    public:
        KeyValue<T>
        operator * () const
        {
            return getKeyValue();
        }

        const Iterator&
        operator ++ ()
        {
            ++offsetIterator;
            return *this;
        }

        bool
        operator == (Iterator &other) const
        {
            return offsetIterator != other.offsetIterator;
        }

        bool
        operator != (Iterator &other) const
        {
            return offsetIterator != other.offsetIterator;
        }

        Iterator(KeyValue<T>* value, const List<std::size_t>& offsets):
            value(value),
            offsetIterator(offsets.begin())
        { }

        Iterator(KeyValue<T>* value, const List<std::size_t>& offsets, bool isEnd):
            value(value),
            offsetIterator(offsets.end())
        { }

    private:

        KeyValue<T>*
        value;

        List<std::size_t>::Iterator
        offsetIterator;

        KeyValue<T>
        getKeyValue() const
        {
            return value[*offsetIterator];
        }
    };

    Iterator
    begin() const;

    Iterator
    end() const;

private:

    KeyValue<T>*
    keys;

    List<KeyValue<T>>
    tempItems;

    int8_t
    associatedValues[128];

    List<std::size_t>
    offsets;

    List<std::size_t>
    columns;

    int8_t
    maxHash = -1;

    std::size_t
    minKeyLength = SSIZE_MAX;

    uint8_t
    getHash(const char *key) const;

    uint8_t
    getHash(const TextSpan *key) const;

    void
    storeTempItems();

    bool
    tryGeneratePerfectHashAlgorithm();

    uint8_t
    getMinHashFromKeys(List<const char *> *keys) const;

    void
    reset();

    bool
    hasUniqueHashes();
};

}

#include "Foundation/Memory/DataStructure/MemoryPooledPerfectHashTableImpl.h"

#endif //FLASHPOINT_MEMORYPOOLEDPERFECTHASHTABLE_H
