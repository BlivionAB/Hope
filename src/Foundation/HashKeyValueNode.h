#ifndef FLASHPOINT_HASH_KEY_VALUE_NODE_H
#define FLASHPOINT_HASH_KEY_VALUE_NODE_H

#include "Foundation/Memory/MemoryPool.h"

using namespace elet::foundation;

template<typename K, typename V>
struct HashKeyValueNode
{
    K
    key;

    V
    value;

    HashKeyValueNode<K, V>*
    next;

    HashKeyValueNode(K key, V value):
        key(key),
        value(value),
        next(nullptr)
    { }

    void*
    operator new(std::size_t size);

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);
};

#include "HashKeyValueNodeImpl.h"

#endif //FLASHPOINT_HASH_KEY_VALUE_NODE_H
