#ifndef FLASHPOINT_HASHTABLE_IMPL_H
#define FLASHPOINT_HASHTABLE_IMPL_H

#include "Foundation/Memory/DataStructure/MemoryPooledPerfectHashTable.h"
#include "Foundation/Memory/TextSpan.h"
#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace apic::foundation::memory::datastructure
{

template<typename T>
void*
MemoryPooledPerfectHashTable<T>::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    return memoryPool->allocate(size, alignof(MemoryPooledPerfectHashTable<T>), ticket);
}

template<typename T>
void*
MemoryPooledPerfectHashTable<T>::operator new(std::size_t size)
{
    return malloc(size);
}

template<typename T>
MemoryPooledPerfectHashTable<T>::MemoryPooledPerfectHashTable(std::initializer_list<std::pair<const char*, T>> list, MemoryPool* memoryPool, MemoryPoolTicket* ticket):
    MemoryPooledPerfectHashTable(list.size(), memoryPool, ticket)
{
    for (const auto& item : list)
    {
        insert(item.first, item.second);

        // Field handlers being deallocated
    }

    index();

    // Field handlers being deallocated
}


template<typename T>
MemoryPooledPerfectHashTable<T>::MemoryPooledPerfectHashTable(std::initializer_list<std::pair<const char*, T>> list):
    MemoryPooledPerfectHashTable(list.size())
{
    for (const auto& item : list)
    {
        insert(item.first, item.second);

        // Field handlers being deallocated
    }

    index();

    // Field handlers being deallocated
}

template<typename T>
MemoryPooledPerfectHashTable<T>::MemoryPooledPerfectHashTable(MemoryPool* memoryPool, MemoryPoolTicket* ticket):
    MemoryPooledPerfectHashTable(1, memoryPool, ticket)
{ }

template<typename T>
MemoryPooledPerfectHashTable<T>::MemoryPooledPerfectHashTable(std::size_t capacity, MemoryPool* memoryPool, MemoryPoolTicket* ticket):
    capacity(capacity)
{
    keys = (KeyValue<T>*)(memoryPool->allocate(sizeof(KeyValue<T>) * capacity, alignof(KeyValue<T>), ticket));
}


template<typename T>
MemoryPooledPerfectHashTable<T>::MemoryPooledPerfectHashTable(std::size_t capacity):
    capacity(capacity)
{
    keys = (KeyValue<T>*)malloc(sizeof(KeyValue<T>) * capacity);
}


template<typename T>
MemoryPooledPerfectHashTable<T>::MemoryPooledPerfectHashTable():
    capacity(0),
    columns()
{ }

template<typename T>
MemoryPooledPerfectHashTable<T>::~MemoryPooledPerfectHashTable()
{ }

template<typename T>
void
MemoryPooledPerfectHashTable<T>::index()
{
    keys = (KeyValue<T>*)calloc(capacity * 2, sizeof(KeyValue<T>));

    for (std::size_t i = 0; i < minKeyLength; i++)
    {
        reset();
        columns.add(i);
        bool success = tryGeneratePerfectHashAlgorithm();
        if (success)
        {
            storeTempItems();
            return;
        }
    }

    for (std::size_t i1 = 0; i1 < minKeyLength; i1++)
    {
        for (std::size_t i2 = 0; i2 < minKeyLength; i2++)
        {
            if (i1 == i2)
            {
                continue;
            }
            reset();
            columns.add(i1);
            columns.add(i2);
            bool success = tryGeneratePerfectHashAlgorithm();
            if (success)
            {
                storeTempItems();
                return;
            }
        }
    }

    throw NonUniqueColumnKeyException("Indexed keys does not contain at least one unique character column.");
}

template<typename T>
void
MemoryPooledPerfectHashTable<T>::storeTempItems()
{
    offsets.clear();
    for (const auto item : tempItems)
    {
        std::size_t hash = getHash(item.key);
        std::memcpy(keys + hash, &item, sizeof(KeyValue<T>));
        offsets.add(hash);
    }
}

template<typename T>
void
MemoryPooledPerfectHashTable<T>::insert(const char *key, const T &value)
{
    std::size_t length = strlen(key);
    if (length < minKeyLength)
    {
        length++;
        minKeyLength = length;
    }
    tempItems.add({ key, value });
}

template<typename T>
bool
MemoryPooledPerfectHashTable<T>::tryGeneratePerfectHashAlgorithm()
{
    /**
     * This algorithm for generating lookup table works as follow:
     *
     *   1) Group key signatures* by common character and sort them by:
     *
     *      1) group size descending order
     *
     *      2) ASCII value ascending order
     *
     *      Example of sorted key signature groups):
     *
     *        Given the following key signatures:
     *
     *           ---    ---    ---    ---
     *          | A |  | A |  | A |  | B |
     *          | B |  | C |  | D |  | E |
     *           ---    ---    ---    ---
     *
     *        The sorted signature groups
     *
     *        Character A):
     *
     *           ---    ---   ---
     *          | A |  | A | | A |
     *          | B |  | C | | D |
     *           ---    ---   ---
     *
     *        Character B):
     *           ---    ---
     *          | A |  | B |
     *          | B |  | E |
     *           ---    ---
     *
     *        Character C):
     *           ---
     *          | A |
     *          | C |
     *           ---
     *
     *        Character D):
     *           ---
     *          | A |
     *          | D |
     *           ---
     *
     *        Character E):
     *           ---
     *          | B |
     *          | E |
     *           ---
     *
     *   2) Assign the associated values* of each character in the order calculated above
     *      by the previous largest hash plus one, starting with hash value of zero. Stop
     *      when the hash value exceeds or is equal to the number of key signatures plus
     *      one.
     *
     *                   hash     hash     hash
     *        AB: A=1    1 B=2    3        3
     *        AC: A=1    1        1 C=4    5
     *        AD: A=1    1        1        1
     *        BE:        0 B=2    2        2
     *
     *      This method guarantees that the entropy always increases on each assignment.
     *      Since, we always begin by assigning '1' instead of '0', the entropy reaches
     *      its maximum when the hash equals number of keys minus 1.
     *
     * Footnotes:
     *
     *   * Key signature of a key is the value of predefined indices in the key. The
     *     signatures are unique for each key. A key 'hello' with key indices 1 and 2
     *     have the key signatures 'e' and 'l'.
     *
     *   * Associated value is the "value" in the character to value mapping in the
     *     lookup table.
     */
    List<IntersectionChar*> intersectionChars;
    for (const KeyValue<T>& item : tempItems)
    {
        const char* key = item.key;
        for (std::size_t index : columns)
        {
            char ch = key[index];
            IntersectionChar* result = intersectionChars.find([&ch](IntersectionChar *intersectionChar)
            {
                return intersectionChar->character == ch;
            });
            if (result)
            {
                result->keys->add(key);
            }
            else
            {
                List<const char*>* keySignatures = new List<const char*>();
                keySignatures->add(key);
                IntersectionChar* intersectionChar = new IntersectionChar(ch, keySignatures);
                intersectionChars.add(intersectionChar);
            }
        }
    }

    intersectionChars.sort([](IntersectionChar* a, IntersectionChar* b)
    {
        if (a->keys->size() > b->keys->size())
        {
           return 1;
        }
        else if (a->keys->size() < b->keys->size())
        {
           return -1;
        }
        return a->character - b->character;
    });

    std::size_t length = tempItems.size();
    int index = 0;
    for (const IntersectionChar* intersectionChar : intersectionChars)
    {
        if (index >= length)
        {
            break;
        }
        associatedValues[intersectionChar->character] = maxHash + 1;

        for (const char* key : *intersectionChar->keys)
        {
            uint8_t hash = getHash(key);
            if (hash > maxHash)
            {
                maxHash = hash;
            }
        }
        index++;
    }
    return hasUniqueHashes();
}


template<typename T>
bool
MemoryPooledPerfectHashTable<T>::hasUniqueHashes()
{
    std::set<unsigned int> hashes;
    for (const KeyValue<T>& item : tempItems)
    {
        const char* key = item.key;
        std::size_t hash = getHash(key);
        auto result = hashes.find(hash);
        if (result != hashes.end())
        {
            return false;
        }
        hashes.insert(hash);
    }
    return true;
}

template<typename T>
uint8_t
MemoryPooledPerfectHashTable<T>::getMinHashFromKeys(List<const char *> *keys) const
{
    std::size_t minHash = UINT8_MAX;
    for (const char* key : *keys)
    {
        uint8_t candidate = getHash(key);
        if (candidate < minHash)
        {
            minHash = candidate;
        }
    }
    return minHash;
}

template<typename T>
void
MemoryPooledPerfectHashTable<T>::reset()
{
    maxHash = 0;
    memset(associatedValues, 0, sizeof(associatedValues));
    columns.clear();
}

template<typename T>
uint8_t
MemoryPooledPerfectHashTable<T>::getHash(const char* key) const
{
    uint8_t hash = 0;
    for (std::size_t i : columns)
    {
        const char ch = key[i];
        hash += associatedValues[ch];
    }
    return hash;
}

template<typename T>
uint8_t
MemoryPooledPerfectHashTable<T>::getHash(const TextSpan *key) const
{
    std::size_t hash = 0;
    for (std::size_t i : columns)
    {
        char ch = (*key)[i];
        hash += associatedValues[ch];
    }
    return hash;
}

template<typename T>
bool
MemoryPooledPerfectHashTable<T>::has(const TextSpan *key) const
{
    int hash = getHash(key);
    if (hash > maxHash)
    {
        return false;
    }
    KeyValue<T>* item = keys + hash;
    if (*(int*)item != 0)
    {
        return *key == item->key;
    }
    return false;
}

template<typename T>
bool
MemoryPooledPerfectHashTable<T>::has(const char *key) const
{
    int hash = getHash(key);
    if (hash > maxHash)
    {
        return false;
    }
    KeyValue<T>* item = keys + hash;
    if (*(int*)item != 0)
    {
        return strcmp(item->key, key) == 0;
    }
    return false;
}

template<typename T>
std::optional<T>
MemoryPooledPerfectHashTable<T>::find(const char *key) const
{
    if (capacity == 0)
    {
        return std::nullopt;
    }
    uint8_t hash = getHash(key);
    void* address = keys + hash;
    if (hash > maxHash)
    {
        return std::nullopt;
    }
    if (*(int*)address == 0)
    {
        return std::nullopt;
    }
    if (strcmp(reinterpret_cast<KeyValue<T>*>(address)->key, key) != 0)
    {
        return std::nullopt;
    }
    return std::optional<T>(reinterpret_cast<KeyValue<T>*>(address)->value);
}

template<typename T>
std::optional<T>
MemoryPooledPerfectHashTable<T>::find(const TextSpan *key) const
{
    if (capacity == 0)
    {
        return std::nullopt;
    }
    std::size_t hash = getHash(key);
    if (hash > maxHash)
    {
        return std::nullopt;
    }
    void* item = keys + hash;
    if (*(int*)item == 0)
    {
        return std::nullopt;
    }
    if ((*key) != reinterpret_cast<KeyValue<T>*>(item)->key)
    {
        return std::nullopt;
    }
    return std::optional<T>(reinterpret_cast<KeyValue<T>*>(item)->value);
}

template<typename T>
std::size_t
MemoryPooledPerfectHashTable<T>::size() const
{
    return 0;
}

template<typename T>
typename MemoryPooledPerfectHashTable<T>::Iterator
MemoryPooledPerfectHashTable<T>::begin() const
{
    return Iterator(keys, offsets);
}

template<typename T>
typename MemoryPooledPerfectHashTable<T>::Iterator
MemoryPooledPerfectHashTable<T>::end() const
{
    return Iterator(keys, offsets, /*isEnd*/true);
}

}

#endif // FLASHPOINT_HASHTABLE_IMPL_H