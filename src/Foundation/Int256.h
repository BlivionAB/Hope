#ifndef ELET_INT256_H
#define ELET_INT256_H

#include <cstdint>
#include <array>


namespace elet::foundation
{
    class Int128;

    class Int256
    {

    public:

        Int256();

        Int256(const Int128& other);

        Int256
        operator -(const Int256& other);

        Int256
        operator +(const Int256& other);

        Int256
        operator <<(int32_t shift);

        Int256
        operator =(const Int128& other);

        bool
        isNegative() const;

    private:

        std::array<uint32_t, 8>
        value;
    };
}
#include "Int128.h"


#endif //ELET_INT256_H
