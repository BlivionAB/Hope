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

    /// Return true whether it's a shifted mask. A shifted mask is a bit pattern with
    /// sequence of ones left-shifted. E.g. 0b00001110 == true.
    bool
    Bit::isShiftedMask64(uint64_t value)
    {
        uint64_t inverseTrailingZeroes = value - 1;
        uint64_t valueAndInverseTrailingZeroUnion = inverseTrailingZeroes | value;
        return value && isMask64(valueAndInverseTrailingZeroUnion);
    }

    /// Return true whether it's a 64 bit mask. A mask is a bit pattern with
    /// sequence of ones beginning from the least significant bit. E.g. 0b00001111 == true.
    bool
    Bit::isMask64(uint64_t value)
    {
        uint64_t inverseOnes = value + 1;
        return value && (inverseOnes & value) == 0;
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