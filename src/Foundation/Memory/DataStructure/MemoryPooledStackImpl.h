#include "MemoryPooledStack.h"

namespace apic::foundation
{

template<typename T>
void*
MemoryPooledStack<T>::operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    return memoryPool->allocate(size, alignof(MemoryPooledStack<T>), ticket);
}

template<typename T>
MemoryPooledStack<T>::MemoryPooledStack()
{ }

template<typename T>
MemoryPooledStack<T>::MemoryPooledStack(MemoryPool* memoryPool, MemoryPoolTicket* ticket):
    MemoryPooledStack(memoryPool, ticket, 1)
{ }

template<typename T>
MemoryPooledStack<T>::MemoryPooledStack(MemoryPool* memoryPool, MemoryPoolTicket* ticket, std::size_t capacity):
    size(0),
    capacity(capacity)
{
    memoryPool = memoryPool;
    ticket = ticket;
    items = reinterpret_cast<T*>(memoryPool->allocate(capacity * sizeof(T), alignof(T), ticket));
    cursor = items;
}

template<typename T>
void
MemoryPooledStack<T>::push(T item)
{
    if (size == capacity)
    {
        reserve(capacity * 2);
    }
    std::memcpy(cursor, &item, sizeof(T));
    cursor++;
    size++;
}

template<typename T>
void
MemoryPooledStack<T>::clear()
{
    cursor = items;
}

template<typename T>
void
MemoryPooledStack<T>::pop()
{
    size--;
    cursor--;
}

template<typename T>
T
MemoryPooledStack<T>::top()
{
    return *(cursor - 1);
}

template<typename T>
void
MemoryPooledStack<T>::reserve(std::size_t capacity)
{
    T* items;
    std::size_t newSize = capacity * sizeof(T);
    items = reinterpret_cast<T*>(memoryPool->allocate(newSize, alignof(T), ticket));
    std::memcpy(items, items, size * sizeof(T));
    std::size_t offset = cursor - items;
    items = items;
    cursor = items + offset;
    this->capacity = capacity;
}

}