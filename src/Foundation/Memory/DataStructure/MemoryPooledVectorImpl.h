#ifndef FLASHPOINT_MEMORY_POOLED_VECTOR_IMPL_H
#define FLASHPOINT_MEMORY_POOLED_VECTOR_IMPL_H

#include "MemoryPooledVector.h"

namespace elet::foundation {

template<typename T>
MemoryPooledVector<T>::MemoryPooledVector():
    capacity(1)
{ }

template<typename T>
MemoryPooledVector<T>::MemoryPooledVector(MemoryPool* memoryPool, MemoryPoolTicket* ticket)
{
    capacity = 1;
    items = (T*) memoryPool->allocate(sizeof(T) * capacity, alignof(T), ticket);
    cursor = items;
    memoryPool = memoryPool;
    memoryPoolTicket = ticket;
}

template<typename T>
void
MemoryPooledVector<T>::add(const T &item)
{
    auto size = sizeof(T);
    std::memcpy(cursor, &item, size);
    cursor++;
}


template<typename T>
void
MemoryPooledVector<T>::add(const T&& item)
{
    if (length() > capacity)
    {
        capacity = capacity * 2;
        T* newItems = (T*) memoryPool->allocate(sizeof(T) * capacity, alignof(T), memoryPoolTicket);
        std::memcpy(items, newItems, capacity * sizeof(T));
        items = newItems;
        cursor = items;
    }
    auto size = sizeof(T);
    std::memcpy(cursor, &item, size);
    cursor++;
}

template<typename T>
std::size_t
MemoryPooledVector<T>::length() const
{
    return cursor - items;
}

template<typename T>
void
MemoryPooledVector<T>::clear()
{
    cursor = items;
}

template<typename T>
bool
MemoryPooledVector<T>::isEmpty() const
{
    return cursor == items;
}

template<typename T>
std::optional<T>
MemoryPooledVector<T>::find(std::function<bool(T &)> predicate)
{
    for (T& item : *this)
    {
        if (predicate(item))
        {
            return item;
        }
    }
    return std::nullopt;
}

template<typename T>
void
MemoryPooledVector<T>::swap(T *a, T *b)
{
    T t = *a;
    *a = *b;
    *b = t;
}

template<typename T>
T&
MemoryPooledVector<T>::operator [](std::size_t index) const
{
    return *(items + index);
}

template<typename T>
int
MemoryPooledVector<T>::partition(int low, int high, std::function<int(T &, T &)> comparator)
{
    T& pivot = (*this)[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (comparator((*this)[j], pivot) <= 0)
        {
            i++;
            swap(&(*this)[i], &(*this)[j]);
        }
    }
    swap(&(*this)[i + 1], &(*this)[high]);
    return (i + 1);
}

template<typename T>
void
MemoryPooledVector<T>::sort(std::function<int(T &, T &)> comparator)
{
    quickSort(0, length() - 1, comparator);
}

template<typename T>
void
MemoryPooledVector<T>::quickSort(int low, int high, std::function<int(T &, T &)> comparator)
{
    if (low < high)
    {
        int pivot = partition(low, high, comparator);
        quickSort(low, pivot - 1, comparator);
        quickSort(pivot + 1, high, comparator);
    }
}

template<typename T>
typename MemoryPooledVector<T>::Iterator
MemoryPooledVector<T>::begin() const
{
    return Iterator(items);
}

template<typename T>
typename MemoryPooledVector<T>::Iterator
MemoryPooledVector<T>::end() const
{
    return Iterator(cursor);
}

template<typename T>
void*
MemoryPooledVector<T>::operator new(std::size_t size, MemoryPool* memory_pool, MemoryPoolTicket* ticket)
{
    return memory_pool->allocate(size, alignof(MemoryPooledVector<T>), ticket);
}

template<typename T>
void
MemoryPooledVector<T>::reserve(std::size_t capacity)
{
    T* items;
    std::size_t newSize = capacity * sizeof(T);
    items = reinterpret_cast<T*>(memoryPool->allocate(newSize, alignof(T), memoryPoolTicket));
    std::memcpy(items, items, length() * sizeof(T));
    items = items;
    cursor = items;
    this->capacity = capacity;
}

}

#endif // FLASHPOINT_MEMORY_POOLED_VECTOR_IMPL_H