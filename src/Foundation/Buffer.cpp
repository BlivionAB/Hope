#include "Buffer.h"

namespace elet::foundation {

Buffer::Buffer(
    const char* value,
    std::size_t size,
    Buffer *next):

    value(value),
    size(size),
    next(next)
{ }

Buffer::Buffer(
    const char* value,
    std::size_t size):

    value(value),
    size(size),
    next(nullptr)
{ }


}