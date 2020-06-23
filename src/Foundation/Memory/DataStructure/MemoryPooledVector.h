#ifndef FLASHPOINT_MEMORY_POOLED_VECTOR_H
#define FLASHPOINT_MEMORY_POOLED_VECTOR_H

#include "Foundation/Memory/MemoryPool.h"
#include <functional>
#include <optional>

namespace elet::foundation
{

template<typename T>
class MemoryPooledVector
{
public:
    struct Result {
        bool
        Success;

        T*
        Value;
    };

    class Iterator {
        friend class MemoryPooledVector;
    public:
        T&
        operator
        *() const
        {
            return *(value);
        }

        const Iterator&
        operator
        ++ ()
        {
            value++;
            return *this;
        }

        bool
        operator
        == (const Iterator& other) const
        {
            return value == other.value;
        }

        bool
        operator
        !=(const Iterator& other) const
        {
            return value != other.value;
        }

        Iterator(T* value):
            value(value)
        { }

    private:

        T*
        value;
    };

    MemoryPooledVector();

    MemoryPooledVector(MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    void*
    operator new(std::size_t size, MemoryPool* memory_pool, MemoryPoolTicket* ticket);

    void
    add(const T& item);

    void
    add(const T&& item);

    T&
    operator
    [](std::size_t index) const;

    std::size_t
    length() const;

    void
    clear();

    bool
    isEmpty() const;

    void
    reserve(std::size_t capacity);

    void
    sort(std::function<int(T&, T&)> comparator);

    std::optional<T>
    find(std::function<bool(T&)> predicate);

    Iterator
    begin() const;

    Iterator
    end() const;

private:

    MemoryPool*
    memoryPool;

    MemoryPoolTicket*
    memoryPoolTicket;

    T*
    items;

    T*
    cursor;

    std::size_t
    capacity;

    void
    swap(T*a, T*b);

    int
    partition(int low, int high, std::function<int(T&, T&)> comparator);

    void
    quickSort(int low, int high, std::function<int(T&, T&)> comparator);

    std::size_t
    getLastIndex();
};

}

#include "MemoryPooledVectorImpl.h"


#endif //FLASHPOINT_MEMORY_POOLED_VECTOR_H
