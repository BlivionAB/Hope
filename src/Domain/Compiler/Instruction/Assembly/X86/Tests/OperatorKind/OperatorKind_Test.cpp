#include "OperatorKindFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(X86OperatorKindFixture, OperatorKind_Add)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add",
            .targets = {
                CompilationTarget::MachO_x86_64,
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_Subtract)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x - y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_MultiplySigned)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x * y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_DivideSigned)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_DivideUnsigned)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_RemainderSigned)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_RemainderUnsigned)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }
}