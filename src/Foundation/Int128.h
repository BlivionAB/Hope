#ifndef ELET_INT128_H
#define ELET_INT128_H

#include <cstdint>
#include <array>
#include "Int256.h"


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

        Int128
        operator /(const Int128& other) const;

        Int128
        operator %(const Int128& other) const;

        DivisionResult
        divide(const Int128& other) const;

        bool
        isEqual(const Int128& other) const;

        bool
        operator <(int32_t other) const;

        bool
        operator <(uint64_t other) const;

        bool
        operator >(uint64_t other) const;

        bool
        operator ==(uint64_t other) const;

        bool
        operator <=(uint64_t other) const;

        bool
        operator >=(uint64_t other) const;

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
        toBool() const;

        uint64_t
        toUint64() const;

        Int128
        onesComplement() const;

    private:

        void
        setBit(uint32_t position, uint32_t bit, Int128& s) const;

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


    bool
    operator ==(const Int128& op1, const Int128& op2);



    Int128
    operator ~(const Int128& op);
}


#endif //ELET_INT128_H
