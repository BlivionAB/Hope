#include "Int256.h"
#include "M64.h"


namespace elet::foundation
{
    Int256::Int256()
    {

    }


    Int256::Int256(const Int128& other)
    {
        if (other.isNegative())
        {
            value =
            {
                other.value[0],
                other.value[1],
                other.value[2],
                other.value[3],
                static_cast<uint32_t>(-1),
                static_cast<uint32_t>(-1),
                static_cast<uint32_t>(-1),
                static_cast<uint32_t>(-1),
            };
        }
        else
        {

            value =
            {
                other.value[0],
                other.value[1],
                other.value[2],
                other.value[3],
                0,
                0,
                0,
                0,
            };
        }
    }


    Int256
    Int256::operator +(const Int256& other)
    {
        Int256 s;
        M64 c;
        c.u64 = static_cast<uint64_t>(value[0]) + static_cast<uint64_t>(other.value[0]);
        s.value[0] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[1]) + static_cast<uint64_t>(other.value[1]) + c.u32.lo;
        s.value[1] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[2]) + static_cast<uint64_t>(other.value[2]) + c.u32.lo;
        s.value[2] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[3]) + static_cast<uint64_t>(other.value[3]) + c.u32.lo;
        s.value[3] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[4]) + static_cast<uint64_t>(other.value[4]) + c.u32.lo;
        s.value[4] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[5]) + static_cast<uint64_t>(other.value[5]) + c.u32.lo;
        s.value[5] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[6]) + static_cast<uint64_t>(other.value[6]) + c.u32.lo;
        s.value[6] = c.u32.lo;
        c.u64 = static_cast<uint64_t>(value[7]) + static_cast<uint64_t>(other.value[7]) + c.u32.lo;
        s.value[7] = c.u32.lo;
        return s;
    }


    Int256
    Int256::operator -(const Int256& other)
    {
        Int256 s;
        M64 c;
        c.s64 = static_cast<int64_t>(value[0]) - static_cast<uint64_t>(other.value[0]);
        s.value[0] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[1]) - static_cast<uint64_t>(other.value[1]) + c.s32.hi;
        s.value[1] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[2]) - static_cast<uint64_t>(other.value[2]) + c.s32.hi;
        s.value[2] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[3]) - static_cast<uint64_t>(other.value[3]) + c.s32.hi;
        s.value[3] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[4]) - static_cast<uint64_t>(other.value[4]) + c.s32.hi;
        s.value[4] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[5]) - static_cast<uint64_t>(other.value[5]) + c.s32.hi;
        s.value[5] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[6]) - static_cast<uint64_t>(other.value[6]) + c.s32.hi;
        s.value[6] = c.s32.lo;
        c.s64 = static_cast<int64_t>(value[7]) - static_cast<uint64_t>(other.value[7]) + c.s32.hi;
        s.value[7] = c.s32.lo;

        return s;
    }


    Int256
    Int256::operator =(const Int128& other)
    {
        other.isNegative();
        return *this;
    }


    Int256
    Int256::operator <<(int32_t shift)
    {
        uint32_t remainder = shift % 32;
        uint32_t large = remainder + 32;
        Int256 s;
        M64 c;
        switch (shift / 32)
        {
            case 0:
                c.u64 = static_cast<uint64_t>(value[7]) << remainder | static_cast<uint64_t>(value[6]) >> (32 - remainder);
                s.value[7] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[6]) << remainder | static_cast<uint64_t>(value[5]) >> (32 - remainder);
                s.value[6] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[5]) << remainder | static_cast<uint64_t>(value[4]) >> (32 - remainder);
                s.value[5] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[4]) << remainder | static_cast<uint64_t>(value[3]) >> (32 - remainder);
                s.value[4] = c.u32.lo;
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
                c.u64 = static_cast<uint64_t>(value[6]) << remainder | static_cast<uint64_t>(value[5]) >> (32 - remainder);
                s.value[7] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[5]) << remainder | static_cast<uint64_t>(value[4]) >> (32 - remainder);
                s.value[6] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[4]) << remainder | static_cast<uint64_t>(value[3]) >> (32 - remainder);
                s.value[5] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[3]) << remainder | static_cast<uint64_t>(value[2]) >> (32 - remainder);
                s.value[4] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[3] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[2] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[1] = c.u32.lo;

                s.value[0] = 0;
                break;

            case 2:
                c.u64 = static_cast<uint64_t>(value[5]) << remainder | static_cast<uint64_t>(value[4]) >> (32 - remainder);
                s.value[7] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[4]) << remainder | static_cast<uint64_t>(value[3]) >> (32 - remainder);
                s.value[6] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[3]) << remainder | static_cast<uint64_t>(value[2]) >> (32 - remainder);
                s.value[5] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[4] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[3] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[2] = c.u32.lo;

                s.value[1] = 0;
                s.value[0] = 0;
                break;

            case 3:
                c.u64 = static_cast<uint64_t>(value[4]) << remainder | static_cast<uint64_t>(value[3]) >> (32 - remainder);
                s.value[7] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[3]) << remainder | static_cast<uint64_t>(value[2]) >> (32 - remainder);
                s.value[6] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[5] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[4] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[3] = c.u32.lo;

                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;

            case 4:
                c.u64 = static_cast<uint64_t>(value[3]) << remainder | static_cast<uint64_t>(value[2]) >> (32 - remainder);
                s.value[7] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[6] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[5] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[4] = c.u32.lo;

                s.value[3] = 0;
                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;

            case 5:
                c.u64 = static_cast<uint64_t>(value[2]) << remainder | static_cast<uint64_t>(value[1]) >> (32 - remainder);
                s.value[7] = c.u32.lo;
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[6] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[5] = c.u32.lo;

                s.value[4] = 0;
                s.value[3] = 0;
                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;


            case 6:
                c.u64 = static_cast<uint64_t>(value[1]) << remainder | static_cast<uint64_t>(value[0]) >> (32 - remainder);
                s.value[7] = c.u32.lo;

                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[6] = c.u32.lo;

                s.value[5] = 0;
                s.value[4] = 0;
                s.value[3] = 0;
                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;

            case 7:
                c.u64 = static_cast<uint64_t>(value[0]) << remainder | static_cast<uint64_t>(value[0]) >> (63 - remainder);
                s.value[7] = c.u32.lo;

                s.value[6] = 0;
                s.value[5] = 0;
                s.value[4] = 0;
                s.value[3] = 0;
                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;

            default:
                s.value[7] = 0;
                s.value[6] = 0;
                s.value[5] = 0;
                s.value[4] = 0;
                s.value[3] = 0;
                s.value[2] = 0;
                s.value[1] = 0;
                s.value[0] = 0;
                break;

        }
        return s;
    }


    bool
    Int256::isNegative() const
    {
        return value[7] & 0x8000'0000;
    }
}
