#include "Int256.h"
#include "Int128.h"
#include "M64.h"
#include <cassert>
#include <cmath>
#include <iostream>

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
        M64 c;
        M64 co1;
        std::array<uint32_t, 8> Z;
        bool thisSign = isNegative();
        bool otherSign = other.isNegative();

        mulSet(Z, value, other.value[0]);
        std::array<uint32_t, 5>* s = reinterpret_cast<std::array<uint32_t, 5>*>(&Z[1]);
        mulAdd(*s, value, other.value[1]);
        s = reinterpret_cast<std::array<uint32_t, 5>*>(&Z[2]);
        mulAdd(*s, value, other.value[2]);
        s = reinterpret_cast<std::array<uint32_t, 5>*>(&Z[3]);
        mulAdd(*s, value, 0x7fffffff & other.value[3]);

        co1.u64 = static_cast<uint64_t>(thisSign) * (static_cast<uint64_t>(~other.value[0]) + 1);
        c.u64 = static_cast<uint64_t>(Z[3]) + (co1.u32.lo << 31);
        Z[3] = c.u32.lo;

        co1.u64 = static_cast<uint64_t>(otherSign) * (static_cast<uint64_t>(~value[0]) + 1);
        c.u64 = static_cast<uint64_t>(Z[3]) + (co1.u32.lo << 31);
        Z[3] = c.u32.lo;

        std::array<uint32_t, 4>* s2 = reinterpret_cast<std::array<uint32_t, 4>*>(&Z[0]);
        return Int128(*s2);
    }


    void
    Int128::mulAdd(std::array<uint32_t, 5>& Z, const std::array<uint32_t, 4>& Y, const uint32_t& X) const
    {
        M64 c;
        c.u64 = static_cast<uint64_t>(Y[0]) * X + Z[0];
        Z[0] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(Y[1]) * X + c.u32.hi + Z[1];
        Z[1] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(Y[2]) * X + c.u32.hi + Z[2];
        Z[2] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(0x7fffffff & Y[3]) * X + Z[3] + c.u32.hi;
        Z[3] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(Z[4]) + c.u32.hi;
        Z[4] = c.u32.lo;
    }

    void
    Int128::mulSet(std::array<uint32_t, 8>& Z, const std::array<uint32_t, 4>& Y, uint32_t X) const
    {
        M64 c;
        c.u64 = static_cast<uint64_t>(Y[0]) * X;
        Z[0] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(Y[1]) * X + c.u32.hi;
        Z[1] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(Y[2]) * X + c.u32.hi;
        Z[2] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(0x7fffffff & Y[3]) * X + c.u32.hi;
        Z[3] = c.u32.lo;
        Z[4] = c.u32.hi;
        Z[5] = 0;
        Z[6] = 0;
        Z[7] = 0;
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
                A = A + Int128(1);
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
                Q = Q + Int128(1);
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


    bool
    Int128::isPositive() const
    {
        return (value[3] & 0x8000'0000) == 0;
    }


    bool Int128::isEqual(const Int128& other) const
    {
        return value[0] == other.value[0]
            && value[1] == other.value[1]
            && value[2] == other.value[2]
            && value[3] == other.value[3];
    }


    bool
    Int128::operator <(const Int128& other) const
    {
        if (isNegative() && other.isPositive())
        {
            return true;
        }
        if (isPositive() && other.isNegative())
        {
            return false;
        }
        if (isPositive() && other.isPositive())
        {
            for (int i = 3; i >= 1; i--)
            {
                if (value[i] < other.value[i])
                {
                    return true;
                }
                if (value[i] > other.value[i])
                {
                    return false;
                }
            }
            return value[0] < other.value[0];
        }


        // Both are negative
        for (int i = 3; i >= 0; i--)
        {
            // A positive value would yield a signbit of 0, rendering it being more negative than s64.
            int32_t s1 = static_cast<int32_t>(value[i]);
            int32_t s2 = static_cast<int32_t>(other.value[i]);
            if (s1 >= 0 && s2 < 0)
            {
                return true;
            }
            if (s2 >= 0 && s1 < 0)
            {
                return false;
            }
            if (s1 < 0 && s2 < 0)
            {
                if (s1 > s2)
                {
                    return false;
                }
                if (s1 < s2)
                {
                    return true;
                }
            }
            else if (s1 >= 0 && s2 >= 0)
            {
                if (s1 > s2)
                {
                    return true;
                }
                if (s1 < s2)
                {
                    return false;
                }
            }
        }
        return false;
    }


    bool
    Int128::operator >(const Int128& other) const
    {
        if (isNegative() && other.isPositive())
        {
            return false;
        }
        if (isPositive() && other.isNegative())
        {
            return true;
        }
        if (isPositive() && other.isPositive())
        {
            for (int i = 3; i >= 1; i--)
            {
                if (value[i] > other.value[i])
                {
                    return true;
                }
                if (value[i] < other.value[i])
                {
                    return false;
                }
            }
            return value[0] > other.value[0];
        }

        // Both are negative
        for (int i = 3; i >= 0; i--)
        {
            // A positive value would yield a signbit of 0, rendering it being more negative than s64.
            int32_t s1 = static_cast<int32_t>(value[i]);
            int32_t s2 = static_cast<int32_t>(other.value[i]);
            if (s1 >= 0 && s2 < 0)
            {
                return false;
            }
            if (s2 >= 0 && s1 < 0)
            {
                return true;
            }
            if (s1 < 0 && s2 < 0)
            {
                if (s1 > s2)
                {
                    return true;
                }
                if (s1 < s2)
                {
                    return false;
                }
            }
            else if (s1 >= 0 && s2 >= 0)
            {
                if (s1 > s2)
                {
                    return false;
                }
                if (s1 < s2)
                {
                    return true;
                }
            }
        }
        return false;
    }


    bool
    Int128::operator >=(const Int128& other) const
    {
        if (*this == other)
        {
            return true;
        }
        return *this > other;
    }


    bool
    Int128::operator <=(const Int128& other) const
    {
        if (*this == other)
        {
            return true;
        }
        return *this < other;
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
            return value[0] < static_cast<uint32_t>(other);
        }
    }


    bool
    Int128::operator <(int64_t other) const
    {
        if (static_cast<int32_t>(value[3]) == -1 && static_cast<int32_t>(value[2]) == -1)
        {
            if (other >= 0)
            {
                return true;
            }
            M64 c;
            c.s64 = other;

            int32_t s1 = static_cast<int32_t>(value[1]);

            // A positive value would yield a signbit of 0, rendering it being more negative than s64.
            if (s1 > 0)
            {
                return true;
            }

            if (s1 < c.s32.hi)
            {
                return true;
            }
            if (s1 > c.s32.hi)
            {
                return false;
            }

            int32_t s0 = static_cast<int32_t>(value[0]);

            // A positive value would yield a signbit of 0, rendering it being more negative than s32.
            if (s0 > 0)
            {
                return true;
            }
            return s0 < c.s32.lo;
        }
        else if (static_cast<int32_t>(value[3]) == 0 && static_cast<int32_t>(value[2]) == 0)
        {
            if (other < 0)
            {
                return false;
            }
            M64 c;
            c.u64 = other;
            if (static_cast<uint32_t>(value[1]) > c.u32.hi)
            {
                return false;
            }
            if (static_cast<uint32_t>(value[1]) < c.u32.hi)
            {
                return true;
            }
            return static_cast<uint32_t>(value[0]) < c.u32.lo;
        }
        else if (static_cast<int32_t>(value[3]) < 0)
        {
            return true;
        }
        else
        {
            return false;
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
    Int128::operator >(int64_t other) const
    {
        if (static_cast<int32_t>(value[3]) == -1 && static_cast<int32_t>(value[2]) == -1)
        {
            if (other >= 0)
            {
                return false;
            }
            M64 c;
            c.s64 = other;

            int32_t s1 = static_cast<int32_t>(value[1]);

            // A positive value would yield a signbit of 0, rendering it being more negative than s64.
            if (s1 > 0)
            {
                return false;
            }

            if (s1 < c.s32.hi)
            {
                return false;
            }
            else if (s1 > c.s32.hi)
            {
                return true;
            }

            int32_t s0 = static_cast<int32_t>(value[0]);

            // A positive value would yield a signbit of 0, rendering it being more negative than s32.
            if (s0 > 0)
            {
                return false;
            }
            return s0 > c.s32.lo;
        }
        else if (static_cast<int32_t>(value[3]) == 0 && static_cast<int32_t>(value[2]) == 0)
        {
            if (other < 0)
            {
                return true;
            }
            M64 c;
            c.u64 = other;
            if (static_cast<uint32_t>(value[1]) > c.u32.hi)
            {
                return true;
            }
            if (static_cast<uint32_t>(value[1]) < c.u32.hi)
            {
                return false;
            }
            return static_cast<uint32_t>(value[0]) > c.u32.lo;
        }
        else if (static_cast<int32_t>(value[3]) < 0)
        {
            return false;
        }
        else
        {
            return true;
        }
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
            else if (value[1] < c.u32.hi)
            {
                return true;
            }
            return value[0] < c.u32.lo;
        }
    }


    bool
    Int128::operator <=(int64_t other) const
    {
        if (*this == other)
        {
            return true;
        }
        return *this < other;
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
    Int128::operator >=(int64_t other) const
    {
        if (*this == other)
        {
            return true;
        }
        return *this > other;
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
    Int128::operator ==(int64_t other) const
    {
        if (!(
            (value[2] == -1 && value[3] == -1) ||
            (value[2] == 0 && value[3] == 0)))
        {
            return false;
        }
        M64 c;
        c.s64 = other;
        if (value[1] != c.s32.hi)
        {
            return false;
        }
        return value[0] == c.s32.lo;
    }


    bool
    Int128::operator !=(const Int128& other) const
    {
        return !(*this == other);
    }


    bool
    Int128::operator ==(const Int128& other) const
    {
        return value[0] == other.value[0]
            && value[1] == other.value[1]
            && value[2] == other.value[2]
            && value[3] == other.value[3];
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
        return res + Int128(1);
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
        return value[0] == 1;
    }


    Int128
    Int128::operator &(const Int128& other) const
    {
        Int128 r;
        r.value[0] = value[0] & other.value[0];
        r.value[1] = value[1] & other.value[1];
        r.value[2] = 0;
        r.value[3] = 0;
        return r;
    }


    Int128
    Int128::operator |(const Int128& other) const
    {
        Int128 r;
        r.value[0] = value[0] | other.value[0];
        r.value[1] = value[1] | other.value[1];
        r.value[2] = 0;
        r.value[3] = 0;
        return r;
    }


    Int128
    Int128::operator ^(const Int128& other) const
    {
        Int128 r;
        r.value[0] = value[0] ^ other.value[0];
        r.value[1] = value[1] ^ other.value[1];
        r.value[2] = 0;
        r.value[3] = 0;
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


    std::string
    Int128::toString() const
    {
        if (isPositive())
        {
            std::string result = "";
            bool encounteredNonZero = false;
            Int128 accumulated = *this;
            for (int e = 38; e >= 0; e--)
            {
                Int128 f = pow(10, e);
                Int128 quotient = accumulated / f;
                if (!encounteredNonZero && quotient != Int128(0))
                {
                    encounteredNonZero = true;
                }
                if (encounteredNonZero)
                {
                    result += std::to_string(quotient.toUint64());
                }
                accumulated = accumulated - quotient * f;
            }
            if (!encounteredNonZero)
            {
                result = "0";
            }
            return result;
        }

        Int128 f = *this;
        Int128 s = -f;
        return "-" + s.toString();
    }

    Int128
    Int128::abs(Int128 v)
    {
        if (v.isNegative())
        {
            return v.onesComplement() + Int128(1);
        }
        return v;
    }


    Int128
    Int128::pow(Int128 a, Int128 b)
    {
        Int128 temp;
        Int128 zero(0);
        if(b == zero)
        {
            return 1;
        }
        temp = pow(a, b / Int128(2));
        if (b % Int128(2) == zero)
        {
            return temp * temp;
        }
        return a * temp * temp;
    }


    Int128
    Int128::max(Int128 a, Int128 b)
    {
        if (a >= b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }


    Int128
    Int128::max(Int128 a, Int128 b, Int128 c)
    {
        return max(max(a, b), c);
    }


    Int128
    Int128::max(Int128 a, Int128 b, Int128 c, Int128 d)
    {
        return max(max(a, b, c), d);
    }


    Int128
    Int128::min(Int128 a, Int128 b)
    {
        if (a <= b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }


    Int128
    Int128::min(Int128 a, Int128 b, Int128 c)
    {
        return min(min(a, b), c);
    }


    Int128
    Int128::min(Int128 a, Int128 b, Int128 c, Int128 d)
    {
        return min(min(a, b, c), d);
    }

    Int128::operator uint32_t() const
    {
        return value[0];
    }
}