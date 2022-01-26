#include "BinaryExpression_ImmediateToImmediateFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_SingleTerm)
    {
        testMainFunction("return 1 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_SingleTerm",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_MultiTerms)
    {
        testMainFunction("return 1 + 1 + 1 + 1 + 1 + 1 + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_MultiTerms",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_LowerPrecedence_HigherPrecedence)
    {
        testMainFunction("return 1 + 2 * 10;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_LowerPrecedence_HigherPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_HigherPrecedence_LowerPrecedence)
    {
        testMainFunction("return 1 * 2 + 10;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_HigherPrecedence_LowerPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_EqualPrecedence)
    {
        testMainFunction("return 1 + 2 + 10;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_EqualPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_LowestPrecedence_MiddlePrecedence_HighestPrecedence)
    {
        testMainFunction("return 1 | 2 ^ 3 & 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_LowestPrecedence_MiddlePrecedence_HighestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_MiddlePrecedence_HighestPrecedence_LowestPrecedence)
    {
        testMainFunction("return 1 ^ 2 & 3 | 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_MiddlePrecedence_HighestPrecedence_LowestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_LowestPrecedence_HighestPrecedence_MiddlePrecedence)
    {
        testMainFunction("return 1 | 2 & 3 ^ 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_LowestPrecedence_HighestPrecedence_MiddlePrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_HighestPrecedence_MiddlePrecedence_LowestPrecedence)
    {
        testMainFunction("return 1 & 2 ^ 3 | 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_HighestPrecedence_MiddlePrecedence_LowestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_HighestPrecedence_LowestPrecedence_MiddlePrecedence)
    {
        testMainFunction("return 1 & 2 | 3 ^ 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_HighestPrecedence_LowestPrecedence_MiddlePrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateToImmediateFixture, BinaryExpression_ImmediateToImmediate_MiddlePrecedence_LowestPrecedence_HighestPrecedence)
    {
        testMainFunction("return 1 ^ 2 | 3 & 4;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateToImmediate_MiddlePrecedence_LowestPrecedence_HighestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

