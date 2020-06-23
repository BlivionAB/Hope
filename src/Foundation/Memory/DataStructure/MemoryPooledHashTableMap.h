#ifndef FLASHPOINT_MEMORY_POOLED_HASH_TABLE_MAP_H
#define FLASHPOINT_MEMORY_POOLED_HASH_TABLE_MAP_H

#include <initializer_list>
#include "Foundation/Memory/Utf8Span.h"
#include "Foundation/HashKeyValueNode.h"
#include "Foundation/FirstCharacterHash.h"

template<typename K, typename V, std::size_t tableSize = 10, typename H = FirstCharacterHash<K>>
class MemoryPooledHashTableMap
{
public:
    MemoryPooledHashTableMap();
    MemoryPooledHashTableMap(MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket1);

    void
    set(const K& key, V value);

    void
    del(const K& key);

    V
    get(const K& key) const;

    V
    get(const char* key) const;

    std::optional<V>
    tryGet(K key) const;

    bool
    has(K key) const;

    bool
    has(const char* key) const;

    std::size_t
    length() const;

    bool
    isEmpty() const;

    class Iterator;

    Iterator
    begin();

    Iterator
    end();

    void
    setMemoryPoolAndTicket(MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

private:

    MemoryPool*
    _memoryPool;

    MemoryPoolTicket*
    _memoryPoolTicket;

    HashKeyValueNode<K, V>**
    _table;

    H
    getHash;

    std::size_t
    _length;

public:

    class Iterator {
        friend class WriteOptimizedSet;
    public:
        V&
        operator
        *() const
        {
            return *currentItem;
        }

        const Iterator&
        operator
        ++ ()
        {
            currentItem = currentItem->next;
            while (currentItem == nullptr && currentBucket < tableSize)
            {
                currentBucket++;
                currentItem = table[currentBucket];
            }
            return *this;
        }

        bool
        operator
        == (const Iterator &other) const
        {
            return currentItem == other.currentItem;
        }

        bool
        operator
        != (const Iterator &other) const
        {
            return currentItem != other.currentItem;
        }

        Iterator(HashKeyValueNode<K, V>** table, std::size_t currentBucket):
            table(table),
            currentBucket(currentBucket)
        { }

    private:

        HashKeyValueNode<K, V>**
        table;

        std::size_t
        currentBucket;

        HashKeyValueNode<K, V>*
        currentItem;
    };
};

#include "MemoryPooledHashTableMapImpl.h"

#endif // FLASHPOINT_MEMORY_POOLED_HASH_TABLE_MAP_H
