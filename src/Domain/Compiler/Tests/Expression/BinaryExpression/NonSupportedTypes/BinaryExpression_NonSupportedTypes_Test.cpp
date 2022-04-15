#include "./BinaryExpression_NonSupportedTypesFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BinaryExpression_NonSupportedTypesFixture, BinaryExpression_NonSupportedTypes_Left)
    {
        testFunction("return true + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_NonSupportedTypes_Left",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_NonSupportedTypesFixture, BinaryExpression_NonSupportedTypes_Right)
    {
        testFunction("return 1 + true;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_NonSupportedTypes_Right",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_NonSupportedTypesFixture, BinaryExpression_NonSupportedTypes_LeftRight)
    {
        testFunction("return true + true;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_NonSupportedTypes_LeftRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

