#ifndef FLASHPOINT_MEMORY_POOLED_HASH_TABLE_MAP_IMPL_H
#define FLASHPOINT_MEMORY_POOLED_HASH_TABLE_MAP_IMPL_H

#include "MemoryPooledHashTableMap.h"
#include "Foundation/HashKeyValueNode.h"

template<typename K, typename V, std::size_t tableSize, typename H>
MemoryPooledHashTableMap<K, V, tableSize, H>::MemoryPooledHashTableMap():
    _table(reinterpret_cast<HashKeyValueNode<K, V>**>(malloc(tableSize * sizeof(_table)))),
    _length(0)
{
    std::memset(_table, 0, sizeof(_table) * tableSize);
}

template<typename K, typename V, std::size_t tableSize, typename H>
MemoryPooledHashTableMap<K, V, tableSize, H>::MemoryPooledHashTableMap(MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket):
    _memoryPool(memoryPool),
    _memoryPoolTicket(memoryPoolTicket),
    _table(reinterpret_cast<HashKeyValueNode<K, V>**>(memoryPool->allocate(tableSize * sizeof(_table), alignof(HashKeyValueNode<K, V>*), memoryPoolTicket))),
    _length(0)
{
    std::memset(_table, 0, sizeof(_table) * tableSize);
}


template<typename K, typename V, std::size_t tableSize, typename H>
void
MemoryPooledHashTableMap<K, V, tableSize, H>::set(const K& key, V value)
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* prev = nullptr;
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr && (item->key) != (key))
    {
        prev = item;
        item = item->next;
    }

    if (item == nullptr)
    {
        item = new (_memoryPool, _memoryPoolTicket) HashKeyValueNode(key, value);
        if (prev == nullptr)
        {
            _table[hash] = item;
        }
        else
        {
            prev->next = item;
        }
        _length++;
    }
    else
    {
        item->value = value;
    }
}

template<typename K, typename V, std::size_t tableSize, typename H>
void
MemoryPooledHashTableMap<K, V, tableSize, H>::del(const K& key)
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* prev = nullptr;
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr && (item->key) != (key))
    {
        prev = item;
        item = item->next;
    }

    if (item == nullptr)
    {
        return;
    }
    else
    {
        if (prev == nullptr)
        {
            _table[hash] = item->next;
        }
        else
        {
            prev->next = item->next;
        }
        delete item;
    }
}

template<typename K, typename V, std::size_t tableSize, typename H>
V
MemoryPooledHashTableMap<K, V, tableSize, H>::get(const K& key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if ((*item->key) == (*key))
        {
            return item->value;
        }
        item = item->next;
    }
    throw std::logic_error(std::string("Could not find key: '") + key->toCharArray() + '"');
}

template<typename K, typename V, std::size_t tableSize, typename H>
V
MemoryPooledHashTableMap<K, V, tableSize, H>::get(const char* key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if (*item->key == key)
        {
            return item->value;
        }
        item = item->next;
    }
}

template<typename K, typename V, std::size_t tableSize, typename H>
std::optional<V>
MemoryPooledHashTableMap<K, V, tableSize, H>::tryGet(K key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if ((*item->key) == (*key))
        {
            return item->value;
        }
        item = item->next;
    }
    throw std::nullopt;
}

template<typename K, typename V, std::size_t tableSize, typename H>
bool
MemoryPooledHashTableMap<K, V, tableSize, H>::has(K key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if (item->key == key)
        {
            return true;
        }
        item = item->next;
    }
    return false;
}

template<typename K, typename V, std::size_t tableSize, typename H>
bool
MemoryPooledHashTableMap<K, V, tableSize, H>::has(const char* key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if (item->key == key)
        {
            return true;
        }
        item = item->next;
    }
    return false;
}

template<typename K, typename V, std::size_t tableSize, typename H>
std::size_t
MemoryPooledHashTableMap<K, V, tableSize, H>::length() const
{
    return _length;
}

template<typename K, typename V, std::size_t tableSize, typename H>
bool
MemoryPooledHashTableMap<K, V, tableSize, H>::isEmpty() const
{
    return _length == 0;
}

template<typename K, typename V, std::size_t tableSize, typename H>
typename MemoryPooledHashTableMap<K, V, tableSize, H>::Iterator
MemoryPooledHashTableMap<K, V, tableSize, H>::begin()
{
    return Iterator(_table, 0);
}

template<typename K, typename V, std::size_t tableSize, typename H>
typename MemoryPooledHashTableMap<K, V, tableSize, H>::Iterator
MemoryPooledHashTableMap<K, V, tableSize, H>::end()
{
    return Iterator(_table, tableSize);
}

template<typename K, typename V, std::size_t tableSize, typename H>
void
MemoryPooledHashTableMap<K, V, tableSize, H>::setMemoryPoolAndTicket(MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket)
{
    _memoryPool = memoryPool;
    _memoryPoolTicket = memoryPoolTicket;
}

template<typename K, typename V, std::size_t tableSize, typename H>
void*
MemoryPooledHashTableMap<K, V, tableSize, H>::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* memoryPoolTicket)
{
    return memoryPool->allocate(size, alignof(MemoryPooledHashTableMap), memoryPoolTicket);
}

#endif // FLASHPOINT_MEMORY_POOLED_HASH_TABLE_MAP_IMPL_H