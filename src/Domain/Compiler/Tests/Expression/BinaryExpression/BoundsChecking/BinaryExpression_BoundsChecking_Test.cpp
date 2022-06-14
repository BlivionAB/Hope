#include "BinaryExpression_BoundsCheckingFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BinaryExpression_BoundsCheckingFixture, Addition)
    {
        testFunction(
            "var x: s32 = 1;\n"
            "var y: s32 = 2;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Addition",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Addition_Signed)
    {
        testFunction(
            "var x: s32 = -1;\n"
            "var y: s32 = -2;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Addition_Signed",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Addition_Overflow)
    {
        testFunction(
            "var x: s32 = 2147483647;\n"
            "var y: s32 = 1;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Addition_Overflow",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Addition_Underflow)
    {
        testFunction(
            "var x: s32 = -2147483648;\n"
            "var y: s32 = -1;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Addition_Underflow",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Subtraction_Unsigned)
    {
        testFunction(
            "var x: s32 = 2;\n"
            "var y: s32 = 1;\n"
            "return x - y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Subtraction_Unsigned",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Subtraction_Signed)
    {
        testFunction(
            "var x: s32 = -2;\n"
            "var y: s32 = -1;\n"
            "return x - y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Subtraction_Signed",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Multiplication)
    {
        testFunction(
            "var x: s32 = 1;\n"
            "var y: s32 = 2;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Multiplication",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Multiplication_Signed)
    {
        testFunction(
            "var x: s32 = -1;\n"
            "var y: s32 = 2;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Multiplication_Signed",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }



    TEST_F(BinaryExpression_BoundsCheckingFixture, Multiplication_Overflow)
    {
        testFunction(
            "var x: s32 = 2;\n"
            "var y: s32 = 2147483647;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Multiplication_Overflow",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Multiplication_Underflow)
    {
        testFunction(
            "var x: s32 = 2;\n"
            "var y: s32 = -2147483648;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Multiplication_Underflow",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }



    TEST_F(BinaryExpression_BoundsCheckingFixture, Division_SignedNoRemainder)
    {
        testFunction(
            "var x: s32 = -4;\n"
            "var y: s32 = 2;\n"
            "return x / y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Division_SignedNoRemainder",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Division_NoRemainder)
    {
        testFunction(
            "var x: s32 = 4;\n"
            "var y: s32 = 2;\n"
            "return x / y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Division_NoRemainder",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }


    TEST_F(BinaryExpression_BoundsCheckingFixture, Division_WithRemainder)
    {
        testFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 2;\n"
            "return x / y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Division_WithRemainder",
            .targets = {
                CompilationTarget::StashIR
            },
            .printTypeBaseline = true
        }));
    }
}

