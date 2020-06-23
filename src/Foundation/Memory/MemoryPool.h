#ifndef FLASHPOINT_REQUEST_ALLOACTOR_H
#define FLASHPOINT_REQUEST_ALLOACTOR_H

#include <stack>
#include <vector>

namespace elet::foundation {

class MemoryPoolTicket {
public:

    std::size_t
    offset;

    std::vector<std::size_t>
    blockIndices;

    std::size_t
    blockIndex;

    MemoryPoolTicket(std::size_t offset, std::size_t block):
        offset(offset),
        blockIndices(block)
    { }
};

class MemoryPool {
public:

    MemoryPool(std::size_t blockSize, std::size_t numberOfBlocks);

    ~MemoryPool();

    void*
    allocate(std::size_t size, std::size_t alignment, MemoryPoolTicket *ticket);

    MemoryPoolTicket*
    takeTicket();

    void
    returnTicket(MemoryPoolTicket *ticket);

    std::size_t
    allocateBlock();

    void
    reset();

    char*
    pool;

private:

    std::size_t
    blockSize;

    std::size_t
    totalSize;

    std::stack<std::size_t>
    freeBlocks;
};

}


#endif //FLASHPOINT_REQUEST_ALLOACTOR_H
