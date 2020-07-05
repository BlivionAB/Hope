#ifndef FLASHPOINT_HASH_TABLE_MAP_H
#define FLASHPOINT_HASH_TABLE_MAP_H

#include "Foundation/Memory/Utf8Span.h"
#include "HashKeyValueNode.h"
#include "FirstCharacterHash.h"
#include <initializer_list>

template<typename K, typename V, std::size_t tableSize = 10, typename H = FirstCharacterHash<K>>
class HashTableMap
{
public:

    HashTableMap();

    HashTableMap(std::initializer_list<std::pair<K, V>> items) noexcept;

    void
    set(const K& key, V value);

    void
    del(const K& key);

    V
    get(const K& key) const;

    template<typename T = K>
    std::optional<V>
    find(T key) const;

    template<typename T = K>
    bool
    has(K key, typename std::enable_if<!std::is_same<T, const char*>::value>::type* = 0) const;

    bool
    has(const char* key) const;

    std::size_t
    size() const;

    void
    clear();

    bool
    isEmpty() const;

    class Iterator;

    Iterator
    begin();

    Iterator
    end();

private:

    HashKeyValueNode<K, V>**
    _table;

    H
    getHash;

    std::size_t
    _length;

public:

    struct KeyValue
    {
        K
        key;

        V
        value;
    };

    class Iterator
    {
        friend class WriteOptimizedSet;
    public:
        KeyValue&
        operator *() const
        {
            return *reinterpret_cast<KeyValue*>(_currentItem);
        }

        const Iterator&
        operator ++ ()
        {
            _currentItem = _currentItem->next;
            while (_currentItem == nullptr && _currentBucket < tableSize)
            {
                _currentBucket++;
                _currentItem = _table[_currentBucket];
            }
            return *this;
        }

        bool
        operator == (const Iterator &other) const
        {
            return _currentItem == other._currentItem;
        }

        bool
        operator != (const Iterator &other) const
        {
            return _currentItem != other._currentItem;
        }

        Iterator(HashKeyValueNode<K, V>** table, std::size_t currentBucket):
            _table(table),
            _currentBucket(currentBucket)
        { }

    private:

        HashKeyValueNode<K, V>**
        _table;

        std::size_t
        _currentBucket;

        HashKeyValueNode<K, V>*
        _currentItem;
    };
};

#include "HashTableMapImpl.h"

#endif // FLASHPOINT_HASH_TABLE_MAP_H
