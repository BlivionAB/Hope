#ifndef ELET_INT128_H
#define ELET_INT128_H

#include <cstdint>
#include <array>
#include "Int256.h"
#include <string>


namespace elet::foundation
{
    struct DivisionResult;

    class Int128
    {

    public:


        Int128();

        Int128(int32_t value);

        Int128(uint32_t value);

        Int128(int64_t value);

        Int128(uint64_t value);

        Int128(std::array<uint32_t, 4> value);

        Int128
        operator +(const Int128& other) const;

        Int128
        operator -(const Int128& other) const;

        Int128
        operator -();

        Int128
        operator *(const Int128& other) const;

        void
        mulSet(std::array<uint32_t, 8>& Z, const std::array<uint32_t, 4>& Y, uint32_t X) const;

        void
        mulAdd(std::array<uint32_t, 5>& Z, const std::array<uint32_t, 4>& Y, const uint32_t& X) const;

        Int128
        operator /(const Int128& other) const;

        Int128
        operator %(const Int128& other) const;

        DivisionResult
        divide(const Int128& other) const;

        bool
        isEqual(const Int128& other) const;

        bool
        operator <(const Int128& other) const;

        bool
        operator >(const Int128& other) const;

        bool
        operator >=(const Int128& other) const;

        bool
        operator <=(const Int128& other) const;

        bool
        operator <(int32_t other) const;

        bool
        operator <(int64_t other) const;

        bool
        operator <(uint64_t other) const;

        bool
        operator >(int64_t other) const;

        bool
        operator >(uint64_t other) const;

        bool
        operator ==(uint64_t other) const;

        bool
        operator ==(int64_t other) const;

        bool
        operator ==(const Int128& other) const;

        bool
        operator !=(const Int128& other) const;

        bool
        operator <=(uint64_t other) const;

        bool
        operator <=(int64_t other) const;

        bool
        operator >=(uint64_t other) const;

        bool
        operator >=(int64_t other) const;

        operator uint32_t() const;

        Int128
        operator &(const Int128& other) const;

        Int128
        operator |(const Int128& other) const;

        Int128
        operator ^(const Int128& other) const;

        Int128
        operator <<(int32_t shift);

        bool
        isNegative() const;

        bool
        isPositive() const;

        bool
        toBool() const;

        std::string
        toString() const;

        uint64_t
        toUint64() const;

        Int128
        onesComplement() const;

        static
        Int128
        abs(Int128 v);

        static
        Int128
        pow(Int128 a, Int128 b);

        static
        Int128
        max(Int128 a, Int128 b);

        static
        Int128
        max(Int128 a, Int128 b, Int128 c);

        static
        Int128
        max(Int128 a, Int128 b, Int128 c, Int128 d);

        static
        Int128
        min(Int128 a, Int128 b);

        static
        Int128
        min(Int128 a, Int128 b, Int128 c);

        static
        Int128
        min(Int128 a, Int128 b, Int128 c, Int128 d);

    private:

        std::array<uint32_t, 4>
        value;

        friend class Int256;
    };


    struct DivisionResult
    {
        Int128
        quotient;

        Int128
        remainder;
    };


    Int128
    operator ~(const Int128& op);
}


#endif //ELET_INT128_H
