#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, BinaryExpression_VariableToImmediate_VariableLeft)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "return x + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_VariableToImmediate_VariableLeft",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_VariableToImmediate_VariableRight)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "return 1 + x;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_VariableToImmediate_VariableRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_VariableToImmediate_CharValue_Left)
    {
        testMainFunction(
            "var x = 'a';\n"
            "return x - 3;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_CharValue_Left",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

