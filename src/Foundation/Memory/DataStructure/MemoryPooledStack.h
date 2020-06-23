#ifndef FLASHPOINT_MEMORY_POOLED_STACK_H
#define FLASHPOINT_MEMORY_POOLED_STACK_H

#include "Foundation/Memory/MemoryPool.h"

namespace apic::foundation {

template<typename T>
class MemoryPooledStack
{
public:

    std::size_t
    size;

    std::size_t
    capacity;

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    MemoryPooledStack();

    MemoryPooledStack(MemoryPool* memoryPool, MemoryPoolTicket* ticket);

    MemoryPooledStack(MemoryPool* memoryPool, MemoryPoolTicket* ticket, std::size_t capacity);

    void
    push(T item);

    void
    pop();

    T
    top();

    void
    clear();

    void
    reserve(std::size_t capacity);

private:

    MemoryPool*
    memoryPool;

    MemoryPoolTicket*
    ticket;

    T*
    items;

    T*
    cursor;
};

}

#include "MemoryPooledStackImpl.h"

#endif //FLASHPOINT_MEMORY_POOLED_STACK_H
