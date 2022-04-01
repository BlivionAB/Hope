#include <gtest/gtest.h>
#include "../Int128.h"


namespace elet::foundation::test
{
    TEST(Int128, Or11)
    {
        Int128 op1({ 1, 1, 1, 1 });
        Int128 op2({ 1, 1, 1, 1 });
        EXPECT_EQ(op1 | op2, Int128({ 1, 1, 1, 1 }));
    }


    TEST(Int128, Or10)
    {
        Int128 op1({ 1, 1, 1, 1 });
        Int128 op2({ 0, 0, 0, 0 });
        EXPECT_EQ(op1 | op2, Int128({ 1, 1, 1, 1 }));
    }


    TEST(Int128, Or01)
    {
        Int128 op1({ 0, 0, 0, 0 });
        Int128 op2({ 1, 1, 1, 1 });
        EXPECT_EQ(op1 | op2, Int128({ 1, 1, 1, 1 }));
    }


    TEST(Int128, Or00)
    {
        Int128 op1({ 0, 0, 0, 0 });
        Int128 op2({ 0, 0, 0, 0 });
        EXPECT_EQ(op1 | op2, Int128({ 0, 0, 0, 0 }));
    }
}