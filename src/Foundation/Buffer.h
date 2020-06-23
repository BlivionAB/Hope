#ifndef FLASHPOINT_BUFFER_H
#define FLASHPOINT_BUFFER_H

#include <Foundation/Memory/MemoryPool.h>

namespace elet::foundation {

struct Buffer
{
    const char*
    value;

    std::size_t
    size;

    Buffer*
    next;

    Buffer(const char* text, std::size_t size);

    Buffer(const char* text, std::size_t size, Buffer *next);

    void*
    operator new(std::size_t size);

    void*
    operator new(std::size_t size, MemoryPool* memoryPool, MemoryPoolTicket* ticket);
};
}


#endif //FLASHPOINT_BUFFER_H
