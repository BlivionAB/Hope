#ifndef FLASHPOINT_HASH_TABLE_MAP_IMPL_H
#define FLASHPOINT_HASH_TABLE_MAP_IMPL_H

#include "HashTableMap.h"
#include "HashKeyValueNode.h"

template<typename K, typename V, std::size_t tableSize, typename H>
HashTableMap<K, V, tableSize, H>::HashTableMap():
    _table(reinterpret_cast<HashKeyValueNode<K, V>**>(calloc(tableSize, sizeof(_table)))),
    _length(0)
{ }

template<typename K, typename V, std::size_t tableSize, typename H>
HashTableMap<K, V, tableSize, H>::HashTableMap(std::initializer_list<std::pair<K, V>> items) noexcept:
    _length(0)
{
    std::size_t size = sizeof(_table);
    _table = reinterpret_cast<HashKeyValueNode<K, V>**>(calloc(tableSize, size));
    for (const std::pair<K, V>& item : items)
    {
        set(item.first, item.second);
    }
}

template<typename K, typename V, std::size_t tableSize, typename H>
void
HashTableMap<K, V, tableSize, H>::set(const K& key, V value)
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* prev = nullptr;
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr && item->key != key)
    {
        prev = item;
        item = item->next;
    }

    if (item == nullptr)
    {
        item = new HashKeyValueNode<K, V>(key, value);
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
HashTableMap<K, V, tableSize, H>::del(const K& key)
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
HashTableMap<K, V, tableSize, H>::get(const K& key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if (item->key == key)
        {
            return item->value;
        }
        item = item->next;
    }
}

template<typename K, typename V, std::size_t tableSize, typename H>
template<typename T>
std::optional<V>
HashTableMap<K, V, tableSize, H>::find(T key) const
{
    std::size_t hash = getHash(key, tableSize);
    HashKeyValueNode<K, V>* item = _table[hash];

    while (item != nullptr)
    {
        if (key == item->key)
        {
            return item->value;
        }
        item = item->next;
    }
    return std::nullopt;
}

template<typename K, typename V, std::size_t tableSize, typename H>
template<typename T>
bool
HashTableMap<K, V, tableSize, H>::has(K key, typename std::enable_if<!std::is_same<T, const char*>::value>::type*) const
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
HashTableMap<K, V, tableSize, H>::size() const
{
    return _length;
}

template<typename K, typename V, std::size_t tableSize, typename H>
bool
HashTableMap<K, V, tableSize, H>::has(const char* key) const
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
void
HashTableMap<K, V, tableSize, H>::clear()
{
    _length = 0;
    std::memcpy(_table, 0, tableSize * sizeof(_table));
}

template<typename K, typename V, std::size_t tableSize, typename H>
bool
HashTableMap<K, V, tableSize, H>::isEmpty() const
{
    return _length == 0;
}

template<typename K, typename V, std::size_t tableSize, typename H>
typename HashTableMap<K, V, tableSize, H>::Iterator
HashTableMap<K, V, tableSize, H>::begin()
{
    return Iterator(_table, 0);
}

template<typename K, typename V, std::size_t tableSize, typename H>
typename HashTableMap<K, V, tableSize, H>::Iterator
HashTableMap<K, V, tableSize, H>::end()
{
    return Iterator(_table, tableSize);
}

#endif // FLASHPOINT_WRITE_OPTIMIZED_MAP_IMPL_H