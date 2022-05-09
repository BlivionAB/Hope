#include "BinaryExpression_BoundedOperationsFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BinaryExpression_BoundedOperationsFixture, Type32_BoundLessThen32)
    {
        testFunction(
            "var x: s32 = 1;\n"
            "var y: s32 = 2;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Type32_BoundLessThen32",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_BoundedOperationsFixture, Type32_Bound32)
    {
        testFunction(
            "var x: s32 = 1;\n"
            "var y: s32 = 2147483646;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Type32_Bound32",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_BoundedOperationsFixture, Type64_Bound32)
    {
        testFunction(
            "var x: s64 = 1;\n"
            "var y: s64 = 2;\n"
            "return x + y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Type64_Bound32",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_BoundedOperationsFixture, Type64_Bound64)
    {
        testFunction(
            "var x: s64 = 1;\n"
            "var y: s64 = 2147483649;\n"
            "return x - y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Type64_Bound64",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

