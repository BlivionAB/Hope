#include <gtest/gtest.h>
#include "../Int128.h"


namespace elet::foundation::test
{
    TEST(Int128, Remainder_PositiveNumeratorPositiveDenominator)
    {
        Int128 op1(7);
        Int128 op2(2);
        EXPECT_EQ(op1 % op2, Int128(1));
    }


    TEST(Int128, Remainder_NegativeNumeratorPositiveDenimonator)
    {
        Int128 op1(-7);
        Int128 op2(2);
        EXPECT_EQ(op1 % op2, Int128(-1));
    }


    TEST(Int128, Remainder_PositiveNumeratorNegativeDenimonator)
    {
        Int128 op1(7);
        Int128 op2(-2);
        EXPECT_EQ(op1 % op2, Int128(-1));
    }


    TEST(Int128, Remainder_NegativeNumeratorNegativeDenimonator)
    {
        Int128 op1(-7);
        Int128 op2(-2);
        EXPECT_EQ(op1 % op2, Int128(-1));
    }


    TEST(Int128, Remainder_Random0)
    {
        Int128 op1(387467823);
        Int128 op2(27364);
        EXPECT_EQ(op1 % op2, Int128(20947));
    }


    TEST(Int128, Remainder_Random1)
    {
        Int128 op1({0,3,0,0});
        Int128 op2({0,2,0,0});
        EXPECT_EQ(op1 % op2, Int128({ 0, 1, 0, 0 }));
    }


    TEST(Int128, Remainder_Random2)
    {
        Int128 op1({0,283756,0,0});
        Int128 op2({0,46,0,0});
        EXPECT_EQ(op1 % op2, Int128({ 0, 28, 0, 0 }));
    }


    TEST(Int128, Remainder_Random3)
    {
        Int128 op1({0,0,0,1});
        Int128 op2({0,0,0,1});
        EXPECT_EQ(op1 % op2, Int128(0));
    }


    TEST(Int128, Remainder_Random4)
    {
        Int128 op1({7,7,7,7});
        Int128 op2({7,7,7,7});
        EXPECT_EQ(op1 % op2, Int128(0));
    }
}