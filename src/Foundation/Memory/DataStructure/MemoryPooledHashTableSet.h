#ifndef FLASHPOINT_MEMORY_POOLED_HASHTABLE_SET_H
#define FLASHPOINT_MEMORY_POOLED_HASHTABLE_SET_H

#include <initializer_list>
#include "Foundation/Memory/Utf8Span.h"
#include "Foundation/HashValueNode.h"
#include "Foundation/FirstCharacterHash.h"

template<typename T, std::size_t tableSize, typename H = FirstCharacterHash<T>>
class MemoryPooledHashTableSet
{
public:
    MemoryPooledHashTableSet(MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket1);

    void
    add(T key);

    bool
    has(T key) const;

    std::size_t
    size() const;

    class Iterator;

    Iterator
    begin();

    Iterator
    end();

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

private:

    MemoryPool*
    memoryPool;

    MemoryPoolTicket*
    memoryPoolTicket;

    HashValueNode<T>**
    table;

    H
    getHash;

    std::size_t
    _size;

public:

    class Iterator {
        friend class WriteOptimizedSet;
    public:
        T&
        operator * () const
        {
            return (*currentItem).immediateValue;
        }

        const Iterator&
        operator ++ ()
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
        operator == (const Iterator &other) const
        {
            return currentItem == other.currentItem;
        }

        bool
        operator != (const Iterator &other) const
        {
            return currentItem != other.currentItem;
        }

        Iterator(HashValueNode<T>** table, std::size_t currentBucket):
            table(table),
            currentBucket(currentBucket)
        { }

    private:

        HashValueNode<T>**
        table;

        std::size_t
        currentBucket;

        HashValueNode<T>*
        currentItem;
    };
};

#include "MemoryPooledHashTableSetImpl.h"

#endif //FLASHPOINT_MEMORY_POOLED_HASHTABLE_SET_H
