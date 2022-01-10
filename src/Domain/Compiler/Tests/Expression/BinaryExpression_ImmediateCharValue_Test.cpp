#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, BinaryExpression_ImmediateCharValue_Left)
    {
        testMainFunction("return '0' + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateCharValue_Left",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, BinaryExpression_ImmediateCharValue_Right)
    {
        testMainFunction("return 2 + '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateCharValue_Right",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

