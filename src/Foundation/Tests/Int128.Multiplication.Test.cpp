#include <gtest/gtest.h>
#include "../Int128.h"


namespace elet::foundation::test
{
    TEST(Int128, Multiplication0)
    {
        Int128 op1({ 2, 0, 0, 0 });
        Int128 op2({ 3, 0, 0, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 6, 0, 0, 0 }));
    }


    TEST(Int128, Multiplication1)
    {
        Int128 op1({ 0, 2, 0, 0 });
        Int128 op2({ 0, 3, 0, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 6, 0 , 0 }));
    }


    TEST(Int128, Multiplication2)
    {
        Int128 op1({ 0, 0, 2, 0 });
        Int128 op2({ 0, 0, 3, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 0, 6, 0 }));
    }


    TEST(Int128, Multiplication3)
    {
        Int128 op1({ 0, 0, 0, 2 });
        Int128 op2({ 0, 0, 0, 3 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 0, 0, 6 }));
    }


    TEST(Int128, Multiplication_NegativePositive)
    {
        Int128 op1(-2);
        Int128 op2(3);
        EXPECT_EQ(op1 * op2, Int128(-6));
    }


    TEST(Int128, Multiplication_PositiveNegative)
    {
        Int128 op1(2);
        Int128 op2(-3);
        EXPECT_EQ(op1 * op2, Int128(-6));
    }


    TEST(Int128, Multiplication_Overflow0)
    {
        Int128 op1({ 0x80000000, 0, 0, 0 });
        Int128 op2({ 2, 0, 0, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 1, 0, 0 }));
    }


    TEST(Int128, Multiplication_Overflow1)
    {
        Int128 op1({ 0, 0x80000000, 0, 0 });
        Int128 op2({ 2, 0, 0, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 0, 1, 0 }));
    }


    TEST(Int128, Multiplication_Overflow2)
    {
        Int128 op1({ 0, 0, 0x80000000, 0 });
        Int128 op2({ 2, 0, 0, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 0, 0, 1 }));
    }


    TEST(Int128, Multiplication_Overflow3)
    {
        Int128 op1({ 0, 0, 0, 0x80000000 });
        Int128 op2({ 2, 0, 0, 0 });
        EXPECT_EQ(op1 * op2, Int128({ 0, 0, 0, 0 }));
    }
}