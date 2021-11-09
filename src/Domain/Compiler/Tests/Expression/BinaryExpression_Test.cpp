#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, BinaryExpression_Variable_2Terms)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Variable_2Terms",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Variable_3Terms)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x + y + z;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Variable_3Terms",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Variable_LowerPrecedenceHigherPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x + y * z;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Variable_LowerPrecedenceHigherPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Variable_LowerPrecedence_HigherPrecedence_HigherThanFirstPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x | y & z ^ x;"); // Note: the precedence order from high to low is &, ^, |.

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Variable_LowerPrecedence_HigherPrecedence_HigherThanFirstPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Variable_LowerPrecedenceHigherPrecedenceLowerPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x + y * z + x;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Variable_LowerPrecedenceHigherPrecedenceLowerPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Variable_HigherPrecedenceLowerPrecedenceHigherPrecedence)
    {
        testMainFunction(
            "var x = 1;\n"
            "var y = 2;\n"
            "var z = 3;\n"
            "return x * y + z * x;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Variable_HigherPrecedenceLowerPrecedenceHigherPrecedence",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpresison_ImmediateValue_LowerThan16Bit)
    {
        testMainFunction("return 1 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpresison_ImmediateValue_LowerThan16Bit",
            .targets = {
                CompilationTarget::StashIR,
                CompilationTarget::MachO_x86_64,
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpresison_ImmediateValue_LargerThan16Bit_BitmaskImmediate_2x32)
    {
        testMainFunction("return 65536 + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpresison_ImmediateValue_LargerThan16Bit_BitmaskImmediate_2x32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpresison_ImmediateValue_LargerThan16Bit_MovzMovk)
    {
        testMainFunction("return 65536 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpresison_ImmediateValue_LargerThan16Bit_MovzMovk",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }
}

