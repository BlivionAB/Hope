#include "DecimalLiteralFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(DecimalLiteralFixture, DecimalLiteral_Positive)
    {
        testFunction("return 2147483647;");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_Positive",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_Zero)
    {
        testFunction("return 0;");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_Zero",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_Negative)
    {
        testFunction("return -2147483648;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_Negative",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_S64NegativeMax)
    {
        testFunction("return -2147483649;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_S64NegativeMax",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_S64NegativeMin)
    {
        testFunction("return -9223372036854775808;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_S64NegativeMin",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_S64MinUnderflow)
    {
        testFunction("return -9223372036854775809;");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_S64MinUnderflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_S64PositiveMin)
    {
        testFunction("return 2147483648;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_S64PositiveMin",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true,
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_S64PositiveMax)
    {
        testFunction("return 9223372036854775807;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_S64PositiveMax",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_S64MaxOverflow)
    {
        testFunction("return 9223372036854775808;");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_S64MaxOverflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_Underline)
    {
        testFunction("return 1_000___000;");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_Underline",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(DecimalLiteralFixture, DecimalLiteral_Error_NonDecimalDigit)
    {
        testFunction("return -1g2;");

        EXPECT_TRUE(testProject({
            .baselineName = "DecimalLiteral_Error_NonDecimalDigit",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}