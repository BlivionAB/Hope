#include "OperatorKindFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(AarchOperatorKindFixture, OperatorKind_Add)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }

    TEST_F(AarchOperatorKindFixture, OperatorKind_Add_S64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x + y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_S64",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }

    TEST_F(AarchOperatorKindFixture, OperatorKind_Subtract)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x - y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Subtract_S64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x - y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_S64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_MultiplySigned)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x * y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_MultiplySigned_S64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x * y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_S64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideSigned)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideSigned_S64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x / y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_S64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
            .acceptBaselines = true
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideUnsigned_U32)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x / y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_U32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideUnsigned_U64)
    {
        testMainFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x / y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_U64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderSigned_S32)
    {
        testMainFunction(
            "var x = 3;\n"
            "var y = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_S32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }



    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderSigned_S64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x % y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_S64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderUnsigned_U32)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_U32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderUnsigned_U64)
    {
        testMainFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x % y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_U64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }
}
