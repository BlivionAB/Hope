#ifndef ELET_M64_H
#define ELET_M64_H


namespace elet::foundation
{
    union M64
    {
        uint64_t
        u64;

        int64_t
        s64;

        struct
        {
            uint32_t
            lo;

            uint32_t
            hi;
        } u32;

        struct
        {
            int32_t
            lo;

            int32_t
            hi;
        } s32;
    };
}

#endif //ELET_M64_H
