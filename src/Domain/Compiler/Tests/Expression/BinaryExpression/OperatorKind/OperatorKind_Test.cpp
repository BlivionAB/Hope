#include "OperatorKindFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(OperatorKindFixture, Operator_Add_32)
    {
        testMainFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Add_32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Add_64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x + y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Add_64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Subtract_32)
    {
        testMainFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x - y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Subtract_32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Subtract_64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x - y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Subtract_64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Multiply_32)
    {
        testMainFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Multiply_32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Multiply_64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x * y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Multiply_64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Divide_S32)
    {
        testMainFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x / y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Divide_S32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Divide_S64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x / y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Divide_S64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }
}

