#include "OperatorKindFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(AarchOperatorKindFixture, OperatorKind_Add_8)
    {
        testMainFunction(
            "var x: s8 = 1 + 2;\n"
            "var y: s8 = 1 + 2;\n"
            "return x + y;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_8",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Add_16)
    {
        testMainFunction(
            "var x: s16 = 1 + 2;\n"
            "var y: s16 = 1 + 2;\n"
            "return x + y;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Add_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x + y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_32",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Add_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x + y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Add_64",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Subtract_8)
    {
        testMainFunction(
            "var x: s8 = 1 + 2;\n"
            "var y: s8 = 1 + 2;\n"
            "return x - y;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Subtract_16)
    {
        testMainFunction(
            "var x: s16 = 1 + 2;\n"
            "var y: s16 = 1 + 2;\n"
            "return x - y;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Subtract_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x - y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_Subtract_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x - y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_Subtract_64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_MultiplySigned_8)
    {
        testMainFunction(
            "var x: s8 = 1 + 2;\n"
            "var y: s8 = 1 + 2;\n"
            "return x * y;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_MultiplySigned_16)
    {
        testMainFunction(
            "var x: s16 = 1 + 2;\n"
            "var y: s16 = 1 + 2;\n"
            "return x * y;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_MultiplySigned_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x * y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_MultiplySigned_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x * y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_MultiplySigned_64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideSigned_8)
    {
        testMainFunction(
            "var x: s8 = 1 + 2;\n"
            "var y: s8 = 1 + 2;\n"
            "return x / y;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideSigned_16)
    {
        testMainFunction(
            "var x: s16 = 1 + 2;\n"
            "var y: s16 = 1 + 2;\n"
            "return x / y;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideSigned_32)
    {
        testMainFunction(
            "var x: s32 = 1 + 2;\n"
            "var y: s32 = 1 + 2;\n"
            "return x / y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideSigned_64)
    {
        testMainFunction(
            "var x: s64 = 1 + 2;\n"
            "var y: s64 = 1 + 2;\n"
            "return x / y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideSigned_64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }

    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideUnsigned_8)
    {
        testMainFunction(
            "var x: u8 = 3;\n"
            "var y: u8 = 3;\n"
            "return x / y;", "u8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideUnsigned_16)
    {
        testMainFunction(
            "var x: u16 = 3;\n"
            "var y: u16 = 3;\n"
            "return x / y;", "u16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideUnsigned_32)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x / y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_DivideUnsigned_64)
    {
        testMainFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x / y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_DivideUnsigned_64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderSigned_8)
    {
        testMainFunction(
            "var x: s8 = 3;\n"
            "var y: s8 = 3;\n"
            "return x % y;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderSigned_16)
    {
        testMainFunction(
            "var x: s16 = 3;\n"
            "var y: s16 = 3;\n"
            "return x % y;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderSigned_32)
    {
        testMainFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderSigned_64)
    {
        testMainFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x % y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderSigned_64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderUnsigned_8)
    {
        testMainFunction(
            "var x: u8 = 3;\n"
            "var y: u8 = 3;\n"
            "return x % y;", "u8");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderUnsigned_16)
    {
        testMainFunction(
            "var x: u16 = 3;\n"
            "var y: u16 = 3;\n"
            "return x % y;", "u16");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderUnsigned_32)
    {
        testMainFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x % y;");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_32",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(AarchOperatorKindFixture, OperatorKind_RemainderUnsigned_64)
    {
        testMainFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x % y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "OperatorKind_RemainderUnsigned_64",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }
}
