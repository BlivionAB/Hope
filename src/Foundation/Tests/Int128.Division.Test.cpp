#include <gtest/gtest.h>
#include "../Int128.h"


namespace elet::foundation::test
{
    TEST(Int128, Division_PositiveNumeratorPositiveDenominator)
    {
        Int128 op1(7);
        Int128 op2(2);
        EXPECT_EQ(op1 / op2, Int128(3));
    }


    TEST(Int128, Division_NegativeNumeratorPositiveDenimonator)
    {
        Int128 op1(-7);
        Int128 op2(2);
        EXPECT_EQ(op1 / op2, Int128(-3));
    }


    TEST(Int128, Division_PositiveNumeratorNegativeDenimonator)
    {
        Int128 op1(7);
        Int128 op2(-2);
        EXPECT_EQ(op1 / op2, Int128(-3));
    }


    TEST(Int128, Division_NegativeNumeratorNegativeDenimonator)
    {
        Int128 op1(-7);
        Int128 op2(-2);
        EXPECT_EQ(op1 / op2, Int128(3));
    }


    TEST(Int128, Division_Random0)
    {
        Int128 op1(387467823);
        Int128 op2(27364);
        EXPECT_EQ(op1 / op2, Int128(14159));
    }


    TEST(Int128, Division_Random1)
    {
        Int128 op1({0,3,0,1});
        Int128 op2({0,2,0,1});
        EXPECT_EQ(op1 / op2, Int128(1));
    }


    TEST(Int128, Division_Random2)
    {
        Int128 op1({0,0,283756,0});
        Int128 op2({0,0,46,0});
        EXPECT_EQ(op1 / op2, Int128(6168));
    }


    TEST(Int128, Division_Random3)
    {
        Int128 op1({0,0,0,1});
        Int128 op2({0,0,0,1});
        EXPECT_EQ(op1 / op2, Int128(1));
    }


    TEST(Int128, Division_Random4)
    {
        Int128 op1({7,7,7,7});
        Int128 op2({7,7,7,7});
        EXPECT_EQ(op1 / op2, Int128(1));
    }
}