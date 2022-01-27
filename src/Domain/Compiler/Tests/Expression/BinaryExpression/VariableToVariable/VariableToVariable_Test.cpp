#include "VariableToVariableFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(VariableToVariableFixture, VariableToVariable_SingleTerm)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_SingleTerm",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_MultiTerms)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x + y + z + x;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_MultiTerms",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_HigherPrecedence_LowerPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x * y + z;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_HigherPrecedence_LowerPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_LowerPrecedence_HigherPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x + y * z;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_LowerPrecedence_HigherPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_EqualPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x + y + z;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_EqualPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_LowestPrecedence_MiddlePrecedence_HighestPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x | y ^ z & x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_LowestPrecedence_MiddlePrecedence_HighestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_MiddlePrecedence_HighestPrecedence_LowestPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x ^ y & z | x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_MiddlePrecedence_HighestPrecedence_LowestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_LowestPrecedence_HighestPrecedence_MiddlePrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x | y & z ^ x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_LowestPrecedence_HighestPrecedence_MiddlePrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_HighestPrecedence_MiddlePrecedence_LowestPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x & y ^ z | x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_HighestPrecedence_MiddlePrecedence_LowestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_HighestPrecedence_LowestPrecedence_MiddlePrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x & y | z ^ x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_HighestPrecedence_LowestPrecedence_MiddlePrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToVariableFixture, VariableToVariable_MiddlePrecedence_LowestPrecedence_HighestPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x ^ y | z & x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToVariable_MiddlePrecedence_LowestPrecedence_HighestPrecedence",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }
}

