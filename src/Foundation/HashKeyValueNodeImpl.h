#ifndef FLASHPOINT_HASH_KEY_VALUE_NODE_IMPL_H
#define FLASHPOINT_HASH_KEY_VALUE_NODE_IMPL_H

#include "HashKeyValueNode.h"

template<typename K, typename V>
void*
HashKeyValueNode<K, V>::operator new(std::size_t size)
{
    return malloc(size);
}

template<typename K, typename V>
void*
HashKeyValueNode<K, V>::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    return memoryPool->allocate(size, alignof(HashKeyValueNode<K, V>), ticket);
}

#endif // FLASHPOINT_HASHVALUE_NODE_IMPL_H