#include "OperatorKindFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(X86OperatorKindFixture, OperatorKind_Add_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_32",
            .targets = {
                CompilationTarget::MachO_x86_64,
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_Add_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x + y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_64",
            .targets = {
                CompilationTarget::MachO_x86_64,
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_Subtract_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x - y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_32",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }



    TEST_F(X86OperatorKindFixture, OperatorKind_Subtract_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x - y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_64",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_MultiplySigned_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_32",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_MultiplySigned_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x * y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_64",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_DivideSigned_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x / y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_32",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_DivideSigned_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x / y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_64",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_DivideUnsigned_32)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x / y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_32",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_DivideUnsigned_64)
    {
        testMainFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x / y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_64",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_RemainderSigned_32)
    {
        testMainFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x % y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_32",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_RemainderSigned_64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x % y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_64",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_RemainderUnsigned_32)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x % y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_32",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }


    TEST_F(X86OperatorKindFixture, OperatorKind_RemainderUnsigned_64)
    {
        testMainFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x % y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_64",
            .targets = {
                CompilationTarget::MachO_x86_64
            },
        }));
    }
}