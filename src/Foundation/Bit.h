#ifndef ELET_BIT_H
#define ELET_BIT_H

#include <cstdint>


namespace elet::foundation
{
    class Bit
    {
    public:

        static
        uint32_t
        countTrailingZeroes(uint64_t imm);


        static
        uint32_t
        countTrailingOnes(uint64_t i);


        static
        uint32_t
        countLeadingOnes(uint64_t value);


        /// Returns true if isMask64 but shifted.
        static
        bool
        isShiftedMask64(uint64_t value);


        /// Returns true if it consists of a non-empty sequence of ones starting with the least significant bit
        static
        bool
        isMask64(uint64_t value);


        // Returns
        static
        unsigned int
        countLeadingZeroes(uint64_t value);


        static
        uint64_t
        rotateRight(uint64_t pattern, unsigned int size);

    };
};



#endif //ELET_BIT_H
