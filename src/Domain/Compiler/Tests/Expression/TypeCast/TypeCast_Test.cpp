#include "TypeCastFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(TypeCastFixture, IntegerLiteralCast)
    {
        testFunction("return 1 as u32;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "IntegerLiteralCast",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_IntegerLiteralCast_Left)
    {
        testFunction("return 1 as u32 + 2;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_IntegerLiteralCast_Left",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_IntegerLiteralCast_Right)
    {
        testFunction("return 1 + 2 as u32;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_IntegerLiteralCast_Right",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_IntegerLiteralCast_3Left)
    {
        testFunction("return 1 as u32 + 2  * 3;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_IntegerLiteralCast_3Left",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_IntegerLiteralCast_3Middle)
    {
        testFunction("return 1 + (2 as u32) * 3;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_IntegerLiteralCast_3Middle",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_IntegerLiteralCast_3Right)
    {
        testFunction("return 1 + 2 * 3 as u32;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_IntegerLiteralCast_3Right",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_MultiCast)
    {
        testFunction("return 1 + 2 * 3 as u32 as u64;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_MultiCast",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_ParenExpressionCast)
    {
        testFunction("return 1 + (2 * 3) as u32;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ParenExpressionCast",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(TypeCastFixture, BinaryExpression_DownCastOverflow)
    {
        testFunction(
            "var x: u64 = 4294967296;\n"
            "return 1 + x as u32;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_DownCastOverflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

