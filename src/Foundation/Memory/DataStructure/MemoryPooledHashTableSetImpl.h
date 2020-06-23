#ifndef FLASHPOINT_WRITE_OPTIMIZED_SET_IMPL_H
#define FLASHPOINT_WRITE_OPTIMIZED_SET_IMPL_H

#include "MemoryPooledHashTableSet.h"
#include "Foundation/HashValueNode.h"

template<typename T, std::size_t tableSize, typename H>
MemoryPooledHashTableSet<T, tableSize, H>::MemoryPooledHashTableSet(MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket):
    memoryPool(memoryPool),
    memoryPoolTicket(memoryPoolTicket),
    table(reinterpret_cast<HashValueNode<T>**>(memoryPool->allocate(tableSize * sizeof(table), alignof(HashValueNode<T>*), memoryPoolTicket))),
    _size(0)
{
    std::memset(table, 0, sizeof(table) * tableSize);
}

template<typename T, std::size_t tableSize, typename H>
void
MemoryPooledHashTableSet<T, tableSize, H>::add(T value)
{
    std::size_t hash = getHash(value, tableSize);
    HashValueNode<T>* prev = nullptr;
    HashValueNode<T>* item = table[hash];

    while (item != nullptr && (*item->value) != (*value))
    {
        prev = item;
        item = item->next;
    }

    if (item == nullptr)
    {
        item = new (memoryPool, memoryPoolTicket) HashValueNode(value);
        if (prev == nullptr)
        {
            table[hash] = item;
        }
        else
        {
            prev->next = item;
        }
        _size++;
    }
}

template<typename T, std::size_t tableSize, typename H>
bool
MemoryPooledHashTableSet<T, tableSize, H>::has(T value) const
{
    std::size_t hash = getHash(value, tableSize);
    HashValueNode<T>* item = table[hash];

    while (item != nullptr)
    {
        if (item->value == value)
        {
            return true;
        }
        item = item->next;
    }
    return false;
}


template<typename T, std::size_t tableSize, typename H>
std::size_t
MemoryPooledHashTableSet<T, tableSize, H>::size() const
{
    return _size;
}

template<typename T, std::size_t tableSize, typename H>
typename MemoryPooledHashTableSet<T, tableSize, H>::Iterator
MemoryPooledHashTableSet<T, tableSize, H>::begin()
{
    return Iterator(table, 0);
}

template<typename T, std::size_t tableSize, typename H>
typename MemoryPooledHashTableSet<T, tableSize, H>::Iterator
MemoryPooledHashTableSet<T, tableSize, H>::end()
{
    return Iterator(table, tableSize);
}

template<typename T, std::size_t tableSize, typename H>
void*
MemoryPooledHashTableSet<T, tableSize, H>::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket)
{
    return memoryPool->allocate(size, alignof(MemoryPooledHashTableSet), memoryPoolTicket);
}

#endif // FLASHPOINT_WRITE_OPTIMIZED_SET_IMPL_H