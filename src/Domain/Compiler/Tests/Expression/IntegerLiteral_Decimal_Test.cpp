#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_Positive)
    {
        testMainFunction("return 2147483647;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_Positive",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_Negative)
    {
        testMainFunction("return -2147483648;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_Negative",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_S64NegativeMax)
    {
        testMainFunction("return -2147483649;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_S64NegativeMax",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_S64NegativeMin)
    {
        testMainFunction("return -9223372036854775808;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_S64NegativeMin",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_S64MinUnderflow)
    {
        testMainFunction("return -9223372036854775809;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_S64MinUnderflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_S64PositiveMin)
    {
        testMainFunction("return 2147483648;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_S64PositiveMin",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_S64Max)
    {
        testMainFunction("return 9223372036854775807;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_S64Max",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_S64MaxOverflow)
    {
        testMainFunction("return 9223372036854775808;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_S64MaxOverflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_Underline)
    {
        testMainFunction("return 1_000___000;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_Underline",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_Error_NonDecimalDigit)
    {
        testMainFunction("return -1g2;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_Error_NonDecimalDigit",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}