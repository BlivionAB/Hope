//
// Created by tinganho on 2021-09-26.
//

#include "Bit.h"
namespace elet::foundation
{

    uint32_t
    Bit::countTrailingZeroes(uint64_t imm)
    {
        int count = 0;
        uint64_t nimm = ~imm;
        for (int i = 0; i < 63; i++)
        {
            if (nimm & (1 << i))
            {
                count++;
                continue;
            }
            break;
        }
        return count;
    }


    uint32_t
    Bit::countTrailingOnes(uint64_t imm)
    {
        int count = 0;
        for (int i = 0; i < 63; i++)
        {
            if (imm & (1 << i))
            {
                count++;
                continue;
            }
            break;
        }
        return count;
    }


    uint32_t
    Bit::countLeadingOnes(uint64_t value)
    {
        int count = 0;
        for (int i = 63; i >= 0; i--)
        {
            if (value & (1 << i))
            {
                count++;
                continue;
            }
            break;
        }
        return count;
    }


    bool
    Bit::isShiftedMask64(uint64_t value)
    {
        return value && isMask64((value - 1 | value));
    }


    bool
    Bit::isMask64(uint64_t value)
    {
        return value && ((value + 1) & value) == 0;
    }


    unsigned int
    Bit::countLeadingZeroes(uint64_t value)
    {
        int count = 0;
        for (int i = 63; i >= 0; i--)
        {
            if (~value & (1 << i))
            {
                count++;
                continue;
            }
            break;
        }
        return count;
    }


    uint64_t
    Bit::rotateRight(uint64_t pattern, unsigned int size)
    {
        return ((pattern & 1) << (size - 1)) | (pattern >> 1);
    }
}