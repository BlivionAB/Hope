#include "OperatorKindFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(OperatorKindFixture, Operator_Add)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Add",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Subtract)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x - y;");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Subtract",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Multiply)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x * y;");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Multiply",
            .targets = {
                CompilationTarget::StashIR,
            }
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Divide)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Divide",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }
}

