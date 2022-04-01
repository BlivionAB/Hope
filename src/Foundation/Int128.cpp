#include "Int256.h"
#include "Int128.h"
#include "M64.h"
#include <cassert>
#include <cmath>

namespace elet::foundation
{
    Int128::Int128():
        value({0, 0, 0, 0})
    {

    }


    Int128::Int128(std::array<uint32_t, 4> value):
        value(value)
    {

    }


    Int128::Int128(int32_t value)
    {
        if (value < 0)
        {
            this->value[3] = -1;
            this->value[2] = -1;
            this->value[1] = -1;
            this->value[0] = value;
        }
        else
        {
            this->value[3] = 0;
            this->value[2] = 0;
            this->value[1] = 0;
            this->value[0] = value;
        }
    }


    Int128::Int128(uint32_t value)
    {
        this->value[3] = 0;
        this->value[2] = 0;
        this->value[1] = 0;
        this->value[0] = value;
    }


    Int128::Int128(int64_t value)
    {
        M64 c;
        c.s64 = value;
        if (value < 0)
        {
            this->value[3] = -1;
            this->value[2] = -1;
            this->value[1] = c.s32.hi;
            this->value[0] = c.s32.lo;
        }
        else
        {
            this->value[3] = 0;
            this->value[2] = 0;
            this->value[1] = c.s32.hi;
            this->value[0] = c.s32.lo;
        }
    }


    Int128::Int128(uint64_t value)
    {
        M64 c;
        c.u64 = value;
        this->value[3] = 0;
        this->value[2] = 0;
        this->value[1] = c.u32.hi;
        this->value[0] = c.u32.lo;
    }


    Int128
    Int128::operator +(const Int128& other) const
    {
        Int128 s;
        M64 c;
        c.u64 = static_cast<uint64_t>(value[0]) + static_cast<uint64_t>(other.value[0]);
        s.value[0] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[1]) + static_cast<uint64_t>(other.value[1]) + c.u32.hi;
        s.value[1] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[2]) + static_cast<uint64_t>(other.value[2]) + c.u32.hi;
        s.value[2] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[3]) + static_cast<uint64_t>(other.value[3]) + c.u32.hi;
        s.value[3] = c.u32.lo;
        return s;
    }


    Int128
    Int128::operator -(const Int128& other) const
    {
        Int128 s;
        M64 c;
        c.s64 = static_cast<int64_t>(value[0]) - other.value[0];
        s.value[0] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[1]) - other.value[1] + c.s32.hi;;
        s.value[1] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[2]) - other.value[2] + c.s32.hi;
        s.value[2] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[3]) - static_cast<uint64_t>(other.value[3]) +  c.u32.hi;
        s.value[3] = c.s32.lo;
        return s;
    }


    Int128
    Int128::operator *(const Int128& other) const
    {
        Int128 s;
        M64 c;
        c.u64 = static_cast<uint64_t>(value[0]) * static_cast<uint64_t>(other.value[0]);
        s.value[0] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[1]) * static_cast<uint64_t>(other.value[0]) + c.u32.hi;
        s.value[1] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[2]) * static_cast<uint64_t>(other.value[0]) + c.u32.hi;
        s.value[2] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[3]) * static_cast<uint64_t>(other.value[0]) + c.u32.hi;
        s.value[3] = c.u32.lo;


        c.u64 = static_cast<uint64_t>(value[0]) * static_cast<uint64_t>(other.value[1]);
        s.value[1] += c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[1]) * static_cast<uint64_t>(other.value[1]) + c.u32.hi;
        s.value[2] += c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[2]) * static_cast<uint64_t>(other.value[1]) + c.u32.hi;
        s.value[3] += c.u32.lo;


        c.u64 = static_cast<uint64_t>(value[0]) * static_cast<uint64_t>(other.value[2]);
        s.value[2] += c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[1]) * static_cast<uint64_t>(other.value[2]) + c.u32.hi;
        s.value[3] += c.u32.lo;


        c.u64 = static_cast<uint64_t>(value[0]) * static_cast<uint64_t>(other.value[3]);
        s.value[3] += c.u32.lo;
        return s;
    }


    Int128
    Int128::operator /(const Int128& other) const
    {
        return divide(other).quotient;
    }


    Int128
    Int128::operator %(const Int128& other) const
    {
        return divide(other).remainder;
    }


    DivisionResult
    Int128::divide(const Int128& other) const
    {
        Int128 A;
        Int128 Q = *this;
        Int128 M = other;
        bool negateQuotient = false;
        bool negateRemainder = false;
        if (Q.isNegative() && !M.isNegative())
        {
            Q = -Q;
            negateQuotient = true;
            negateRemainder = true;
        }
        else if (!Q.isNegative() && M.isNegative())
        {
            M = -M;
            negateQuotient = true;
            negateRemainder = true;
        }
        else if (Q.isNegative() && M.isNegative())
        {
            Q = -Q;
            M = -M;
            negateRemainder = true;
        }
        bool success = true;

        for (int i = 127; i >= 0; i--)
        {
            A = A << 1;
            if (Q.isNegative())
            {
                A = A + 1;
            }
            if (success)
            {
                A = A - M;
            }
            else
            {
                A = A + M;
            }
            if (A.isNegative())
            {
                Q = Q << 1;
                success = false;
            }
            else
            {
                Q = Q << 1;
                Q = Q + 1;
                success = true;
            }
        }
        if (A.isNegative())
        {
            A = A + M;
        }
        if (negateRemainder)
        {
            A = -A;
        }
        if (negateQuotient)
        {
            Q = -Q;
        }
        return { Q, A };
    }


    bool
    Int128::isNegative() const
    {
        return value[3] & 0x8000'0000;
    }


    void
    Int128::setBit(uint32_t position, uint32_t bit, Int128& s) const
    {
        assert(position < 128 && "position must be smaller than 128.");
        assert(bit == 0 || bit == 1 && "bit must be  0 or 1.");

        if (position < 32)
        {
            s.value[0] |= bit << position;
        }
        else if (position < 64)
        {
            s.value[1] |= bit << (position - 32);
        }
        else if (position < 96)
        {
            s.value[2] |= bit << (position - 64);
        }
        else
        {
            s.value[3] |= bit << (position - 96);
        }
    }


    bool Int128::isEqual(const Int128& other) const
    {
        return value[0] == other.value[0]
            && value[1] == other.value[1]
            && value[2] == other.value[2]
            && value[3] == other.value[3];
    }



    bool
    Int128::operator <(int32_t other) const
    {
        if (isNegative())
        {
            if (static_cast<int32_t>(value[1]) != -1)
            {
                return false;
            }
            if (static_cast<int32_t>(value[2]) != -1)
            {
                return false;
            }
            if (static_cast<int32_t>(value[3]) != -1)
            {
                return false;
            }
            return static_cast<int32_t>(value[0]) < other;
        }
        else
        {
            if (value[1] > 0)
            {
                return false;
            }
            if (value[2] > 0)
            {
                return false;
            }
            if (value[3] > 0)
            {
                return false;
            }
            return value[0] < other;
        }
    }


    bool
    Int128::operator >(uint64_t other) const
    {
        if (isNegative())
        {
            return false;
        }
        if (static_cast<uint32_t>(value[3]) != 0)
        {
            return true;
        }
        if (static_cast<uint32_t>(value[2]) != 0)
        {
            return true;
        }
        M64 c;
        c.u64 = other;
        if (value[1] > c.u32.hi)
        {
            return true;
        }
        return value[0] > c.u32.lo;
    }


    bool
    Int128::operator <(uint64_t other) const
    {
        if (isNegative())
        {
            return true;
        }
        else
        {
            if (value[2] > 0)
            {
                return false;
            }
            if (value[3] > 0)
            {
                return false;
            }
            M64 c;
            c.u64 = other;
            if (value[1] > c.u32.hi)
            {
                return false;
            }
            return value[0] < c.u32.lo;
        }
    }

    bool
    Int128::operator <=(uint64_t other) const
    {
        if (*this == other)
        {
            return true;
        }
        return *this < other;
    }


    bool
    Int128::operator ==(uint64_t other) const
    {
        if (value[2] != 0)
        {
            return false;
        }
        if (value[3] != 0)
        {
            return false;
        }
        M64 c;
        c.u64 = other;
        if (value[1] != c.u32.hi)
        {
            return false;
        }
        return value[0] == c.u32.lo;
    }


    bool
    Int128::operator >=(uint64_t other) const
    {
        if (*this == other)
        {
            return true;
        }
        return *this > other;
    }


    Int128
    Int128::operator -()
    {
        std::array<uint32_t, 4> v;
        v[3] = ~value[3];
        v[2] = ~value[2];
        v[1] = ~value[1];
        v[0] = ~value[0];
        Int128 res(v);
        return res + 1;
    }


    bool
    Int128::toBool() const
    {
        if (value[3] != 0)
        {
            return false;
        }
        if (value[2] != 0)
        {
            return false;
        }
        if (value[1] != 0)
        {
            return false;
        }
        return value[0] == 0;
    }


    Int128
    Int128::operator&(const Int128& other) const
    {
        Int128 r;
        r.value[0] = value[0] & other.value[0];
        r.value[1] = value[1] & other.value[1];
        r.value[2] = value[2] & other.value[2];
        r.value[3] = value[3] & other.value[3];
        return r;
    }


    Int128
    Int128::operator|(const Int128& other) const
    {
        Int128 r;
        r.value[0] = value[0] | other.value[0];
        r.value[1] = value[1] | other.value[1];
        r.value[2] = value[2] | other.value[2];
        r.value[3] = value[3] | other.value[3];
        return r;
    }


    Int128
    Int128::operator^(const Int128& other) const
    {
        Int128 r;
        r.value[0] = value[0] ^ other.value[0];
        r.value[1] = value[1] ^ other.value[1];
        r.value[2] = value[2] ^ other.value[2];
        r.value[3] = value[3] ^ other.value[3];
        return r;
    }


    uint64_t
    Int128::toUint64() const
    {
        M64 c;
        c.u32.lo = value[0];
        c.u32.hi = value[1];
        return c.u64;
    }

    Int128
    Int128::onesComplement() const
    {
        Int128 s({
            ~value[0],
            ~value[1],
            ~value[2],
            ~value[3]
        });

        return s;
    }


    bool
    operator ==(const Int128& op1, const Int128& op2)
    {
        return op1.isEqual(op2);
    }


    Int128
    operator ~(const Int128& op)
    {
        return op.onesComplement();
    }



    Int128
    Int128::operator <<(int32_t shift)
    {
        uint32_t remainder = shift % 32;
        uint32_t large = remainder + 32;
        Int128 s;
        M64 c;
        switch (shift / 32)
        {
            case 0:
                c.u64 = static_cast<uint64_t>(value[3]) << remainder | static_cast<uint64_t>(value[2]) >> (32 - remainder);
                s.value[3] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[2] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[1] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[0] = c.u32.lo;
                break;

            case 1:
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[3] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[2] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[1] = c.u32.lo;

                s.value[0] = 0;
                break;


            case 2:
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[3] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[2] = c.u32.lo;

                s.value[1] = 0;
                s.value[0] = 0;
                break;

            case 3:
                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[3] = c.u32.lo;

                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;
            default:
                s.value[3] = 0;
                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;
        }
        return s;
    }
}