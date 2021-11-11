#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_LowerThan16Bit)
    {
        testMainFunction("return 1 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_LowerThan16Bit",
            .targets = {
                CompilationTarget::StashIR,
                CompilationTarget::MachO_x86_64,
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_LargerThan16Bit_BitmaskImmediate_2x32)
    {
        testMainFunction("return 65536 + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_LargerThan16Bit_BitmaskImmediate_2x32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_LargerThan16Bit_MovzMovk)
    {
        testMainFunction("return 65536 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_LargerThan16Bit_MovzMovk",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_SingleTerm)
    {
        testMainFunction("return 1 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_SingleTerm",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_MultiTerms)
    {
        testMainFunction("return 1 + 1 + 1 + 1 + 1 + 1 + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_MultiTerms",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_LowerPrecedence_HigherPrecedence)
    {
        testMainFunction("return 1 + 2 * 10;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_LowerPrecedence_HigherPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_HigherPrecedence_LowerPrecedence)
    {
        testMainFunction("return 1 * 2 + 10;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_HigherPrecedence_LowerPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_EqualPrecedence)
    {
        testMainFunction("return 1 + 2 + 10;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_EqualPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_LowestPrecedence_MiddlePrecedence_HighestPrecedence)
    {
        testMainFunction("return 1 | 2 ^ 3 & 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_LowestPrecedence_MiddlePrecedence_HighestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_MiddlePrecedence_HighestPrecedence_LowestPrecedence)
    {
        testMainFunction("return 1 ^ 2 & 3 | 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_MiddlePrecedence_HighestPrecedence_LowestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_LowestPrecedence_HighestPrecedence_MiddlePrecedence)
    {
        testMainFunction("return 1 | 2 & 3 ^ 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_LowestPrecedence_HighestPrecedence_MiddlePrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_HighestPrecedence_MiddlePrecedence_LowestPrecedence)
    {
        testMainFunction("return 1 & 2 ^ 3 | 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_HighestPrecedence_MiddlePrecedence_LowestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_HighestPrecedence_LowestPrecedence_MiddlePrecedence)
    {
        testMainFunction("return 1 & 2 | 3 ^ 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_HighestPrecedence_LowestPrecedence_MiddlePrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateValue_MiddlePrecedence_LowestPrecedence_HighestPrecedence)
    {
        testMainFunction("return 1 ^ 2 | 3 & 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateValue_MiddlePrecedence_LowestPrecedence_HighestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

