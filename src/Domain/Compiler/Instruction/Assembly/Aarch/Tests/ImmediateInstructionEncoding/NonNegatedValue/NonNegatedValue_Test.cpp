#include "NonNegatedValueFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(NonNegatedValueFixture, NonNegatedValue_0)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_16)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_16_0)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0011;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0000_0000_0000_0000_0000;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32_0)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0000_0000_0000_0000_0011;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32_16)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0011_0000_0000_0000_0000;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32_16_0)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0011_0000_0000_0000_0111;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000____0000_0000_0000_0000_0000_0000_0000_0000;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48_0)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000____0000_0000_0000_0000_0000_0000_0000_0011;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48_16_0)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000____0000_0000_0000_0111_0000_0000_0000_0011;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48_32_16_0)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_1111____0000_0000_0000_0111_0000_0000_0000_0011;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48_32_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }
}