#include <gtest/gtest.h>
#include "../Int128.h"


namespace elet::foundation::test
{
    TEST(Int128, Subtraction0)
    {
        Int128 op1({ 1, 0,0,0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 - op2, Int128({ 0, 0, 0, 0 }));
    }


    TEST(Int128, Subtraction1)
    {
        Int128 op1({0, 1, 0, 0 });
        Int128 op2({ 0, 1 , 0, 0 });
        EXPECT_EQ(op1 - op2, Int128({ 0, 0, 0 , 0 }));
    }


    TEST(Int128, Subtraction2)
    {
        Int128 op1({0, 0, 1, 0 });
        Int128 op2({ 0, 0 , 1, 0 });
        EXPECT_EQ(op1 - op2, Int128({ 0, 0, 0, 0 }));
    }


    TEST(Int128, Subtraction3)
    {
        Int128 op1({0, 0, 0, 1 });
        Int128 op2({ 0, 0 , 0, 1 });
        EXPECT_EQ(op1 - op2, Int128({ 0, 0, 0, 0 }));
    }


    TEST(Int128, Subtraction_Underflow0)
    {
        Int128 op1({ 0, 0, 0, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 - op2, Int128({ UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX }));
    }


    TEST(Int128, Subtraction_Underflow1)
    {
        Int128 op1({ 0, 1, 0, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 - op2, Int128({ UINT32_MAX, 0, 0, 0 }));
    }


    TEST(Int128, Subtraction_Underflow2)
    {
        Int128 op1({ 0, 0, 1, 0 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 - op2, Int128({ UINT32_MAX, UINT32_MAX, 0, 0 }));
    }


    TEST(Int128, Subtraction_Underflow3)
    {
        Int128 op1({ 0, 0, 0, 1 });
        Int128 op2({ 1, 0, 0, 0 });
        EXPECT_EQ(op1 - op2, Int128({ UINT32_MAX, UINT32_MAX, UINT32_MAX, 0 }));
    }
}