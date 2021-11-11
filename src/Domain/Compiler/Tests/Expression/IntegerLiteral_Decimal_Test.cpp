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


    TEST_F(ExpressionFixture, IntegerLiteral_Decimal_Error_S32MaxOverflow)
    {
        testMainFunction("return 2147483648;");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteral_Decimal_Error_S32MaxOverflow",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
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