#include "MemoryPool.h"
#include <memory>
#include <iostream>

#define start_address_of_block(x)

namespace elet::foundation {

MemoryPool::MemoryPool(std::size_t blockSize, std::size_t numberOfBlocks):
    blockSize(blockSize),
    totalSize(blockSize * numberOfBlocks)
{
    pool = (char*)malloc(totalSize);
    if (pool == nullptr)
    {
        exit (1);
    }
#if DEBUG
    printf("Allocated Pool Address: from: %p to %p\n", pool, pool + totalSize);
#endif
    freeBlocks = {};
    reset();
}

MemoryPool::~MemoryPool()
{
#if DEBUG
    printf("Deallocating Pool Address: from: %p to %p\n", pool, pool + totalSize);
#endif
    delete pool;
}

std::size_t
MemoryPool::allocateBlock()
{
    std::size_t block = freeBlocks.top();
    freeBlocks.pop();
    return block;
}

MemoryPoolTicket*
MemoryPool::takeTicket()
{
    std::size_t blockIndex = allocateBlock();
    MemoryPoolTicket* ticket = (MemoryPoolTicket*)(pool + blockIndex * blockSize);
    ticket->blockIndices.push_back(blockIndex);
    ticket->blockIndex = blockIndex;
    ticket->offset = sizeof(MemoryPoolTicket);
    return ticket;
}

void
MemoryPool::returnTicket(MemoryPoolTicket *ticket)
{
    for (const auto& block_index : ticket->blockIndices)
    {
        freeBlocks.push(block_index);
    }
}

void*
MemoryPool::allocate(std::size_t size, std::size_t alignment, MemoryPoolTicket *ticket)
{
    std::size_t padding = 0;
    std::size_t currentAddress = (std::size_t)(pool + ticket->blockIndex * blockSize + ticket->offset);

    if (alignment != 0 && currentAddress % alignment != 0)
    {
        const std::size_t multiplier = (currentAddress / alignment) + 1;
        const std::size_t alignedAddress = multiplier * alignment;
        padding = alignedAddress - currentAddress;
    }

    if (ticket->offset + padding + size > blockSize)
    {
        auto blockIndex = allocateBlock();
        ticket->blockIndices.push_back(blockIndex);
        ticket->blockIndex = blockIndex;
        ticket->offset = 0;
        if (blockIndex == -1)
        {
            throw std::logic_error("Out of memory: trying to allocate a block of memory for a request.");
        }
        return allocate(size, alignment, ticket);
    }

    ticket->offset += padding + size;

#ifdef DEBUG
    std::cout << "A" << "\t@C " << (void*) currentAddress << "\t@R " << "\tO " << ticket->offset << "\tP " << padding << std::endl;
#endif

    return (void*)currentAddress;
}

void
MemoryPool::reset()
{
    std::size_t blocks = totalSize / blockSize;
    for (std::size_t i = 0; i < blocks; i++)
    {
        freeBlocks.push(i);
    }
}

}