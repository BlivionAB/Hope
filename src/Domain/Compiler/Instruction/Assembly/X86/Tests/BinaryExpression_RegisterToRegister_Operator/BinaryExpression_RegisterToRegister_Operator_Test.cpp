#include "BinaryExpression_RegisterToRegister_Operator_Fixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_Add)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_Add",
            .targets = {
                CompilationTarget::MachO_x86_64,
            },
        }));
    }


    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_Subtract)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x - y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_Subtract",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_MultiplySigned)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x * y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_MultiplySigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_DivideSigned)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_DivideSigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_DivideUnsigned)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_DivideUnsigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_RemainderSigned)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_RemainderSigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(BinaryExpression_RegisterToRegister_Operator_Fixture, BinaryExpression_RegisterToRegister_Operator_RemainderUnsigned)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_RegisterToRegister_Operator_RemainderUnsigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }
}