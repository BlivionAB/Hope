#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, BinaryExpression_Operator_Add)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Operator_Add",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Operator_Subtract)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x - y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Operator_Subtract",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Operator_Multiply)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x * y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Operator_Multiply",
            .targets = {
                CompilationTarget::StashIR,
            }
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_Operator_Divide)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_Operator_Divide",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }
}

