#include <gtest/gtest.h>
#include "../Int128.h"


namespace elet::foundation::test
{
    TEST(Int128, Addition0)
    {
        Int128 op1({ 1, 0,0,0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 2, 0, 0, 0 }));
    }


    TEST(Int128, Addition1)
    {
        Int128 op1({0, 1, 0, 0 });
        Int128 op2({ 0, 1 , 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 2, 0 , 0 }));
    }


    TEST(Int128, Addition2)
    {
        Int128 op1({0, 0, 1, 0 });
        Int128 op2({ 0, 0 , 1, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 0, 2, 0 }));
    }


    TEST(Int128, Addition3)
    {
        Int128 op1({0, 0, 0, 1 });
        Int128 op2({ 0, 0 , 0, 1 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 0, 0, 2 }));
    }


    TEST(Int128, Addition_Overflow0)
    {
        Int128 op1({UINT32_MAX, 0, 0, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 1, 0, 0 }));
    }


    TEST(Int128, Addition_Overflow1)
    {
        Int128 op1({UINT32_MAX, 0, 0, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 1, 0, 0 }));
    }


    TEST(Int128, Addition_Overflow2)
    {
        Int128 op1({UINT32_MAX, UINT32_MAX, 0, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 0, 1, 0 }));
    }


    TEST(Int128, Addition_Overflow3)
    {
        Int128 op1({UINT32_MAX, UINT32_MAX, UINT32_MAX, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 0, 0, 1 }));
    }


    TEST(Int128, Addition_Overflow4)
    {
        Int128 op1({ UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 + op2, Int128({ 0, 0, 0, 0 }));
    }
}