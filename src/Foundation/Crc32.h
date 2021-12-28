#ifndef ELET_CRC32_H
#define ELET_CRC32_H


#include <cstdint>
#include "./List.h"


namespace elet::foundation
{
    uint32_t
    crc32(uint32_t crc, const List<uint8_t>& data);


    uint32_t
    crc32(const List<uint8_t>& data);

    // Inspiration from LLVM source code:
    class Crc32
    {
    public:
        Crc32(uint32_t init = 0xFFFFFFFFU);

        void
        update(const List<uint8_t>& data);

        uint32_t
        value() const;

    private:
        uint32_t crc;
    };
}


#endif //ELET_CRC32_H
