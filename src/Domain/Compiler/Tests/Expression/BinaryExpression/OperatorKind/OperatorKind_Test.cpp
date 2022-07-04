#include "OperatorKindFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(OperatorKindFixture, Operator_Add_32)
    {
        testFunction(
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
        testFunction(
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
        testFunction(
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
        testFunction(
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
        testFunction(
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
        testFunction(
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
        testFunction(
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
        testFunction(
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


    TEST_F(OperatorKindFixture, Operator_Divide_U32)
    {
        testFunction(
            "var x: u32 = 3;\n"
            "var y: u32 = 3;\n"
            "return x / y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Divide_U32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Divide_U64)
    {
        testFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x / y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Divide_U64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Remainder_S32_TreatModAsDiv)
    {
        testFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x % y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Remainder_S32_TreatModAsDiv",
            .targets = {
                CompilationTarget::StashIR,
            },
            .stashIrOptions = StashIrOption {
                .treatModuloAsDivision = true
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Remainder_S32_Msub)
    {
        testFunction(
            "var x: s32 = 3;\n"
            "var y: s32 = 3;\n"
            "return x % y;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Remainder_S32_Msub",
            .targets = {
                CompilationTarget::StashIR,
            },
            .stashIrOptions = StashIrOption {
                .treatModuloAsDivision = false
            }
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Remainder_S64_TreatModAsDiv)
    {
        testFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x % y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Remainder_S64_TreatModAsDiv",
            .targets = {
                CompilationTarget::StashIR,
            },
            .stashIrOptions = StashIrOption {
                .treatModuloAsDivision = true
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Remainder_S64_Msub)
    {
        testFunction(
            "var x: s64 = 3;\n"
            "var y: s64 = 3;\n"
            "return x % y;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Remainder_S64_Msub",
            .targets = {
                CompilationTarget::StashIR,
            },
            .stashIrOptions = StashIrOption {
                .treatModuloAsDivision = false
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Remainder_U64_TreatModAsDiv)
    {
        testFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x % y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Remainder_U64_TreatModAsDiv",
            .targets = {
                CompilationTarget::StashIR,
            },
            .stashIrOptions = StashIrOption {
                .treatModuloAsDivision = true
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Remainder_U64_Msub)
    {
        testFunction(
            "var x: u64 = 3;\n"
            "var y: u64 = 3;\n"
            "return x % y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Remainder_U64_Msub",
            .targets = {
                CompilationTarget::StashIR,
            },
            .stashIrOptions = StashIrOption {
                .treatModuloAsDivision = false
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_And_U32)
    {
        testFunction(
            "var x: u32 = 0b0001;\n"
            "var y: u32 = 0b1000;\n"
            "return x & y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_And_U32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_And_U64)
    {
        testFunction(
            "var x: u64 = 0b0001;\n"
            "var y: u64 = 0b1000;\n"
            "return x & y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_And_U64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Or_U32)
    {
        testFunction(
            "var x: u32 = 0b0001;\n"
            "var y: u32 = 0b1000;\n"
            "return x | y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Or_U32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Or_U64)
    {
        testFunction(
            "var x: u64 = 0b0001;\n"
            "var y: u64 = 0b1000;\n"
            "return x | y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Or_U64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Xor_U32)
    {
        testFunction(
            "var x: u32 = 0b0001;\n"
            "var y: u32 = 0b1000;\n"
            "return x ^ y;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Xor_U32",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }


    TEST_F(OperatorKindFixture, Operator_Xor_U64)
    {
        testFunction(
            "var x: u64 = 0b0001;\n"
            "var y: u64 = 0b1000;\n"
            "return x ^ y;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "Operator_Xor_U64",
            .targets = {
                CompilationTarget::StashIR,
            },
        }));
    }
}

