#include "NonNegatedValueFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(NonNegatedValueFixture, NonNegatedValue_0)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_16)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_16_0)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0011;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0000_0000_0000_0000_0000;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32_0)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0000_0000_0000_0000_0011;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32_16)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0011_0000_0000_0000_0000;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_32_16_0)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0001____0000_0000_0000_0011_0000_0000_0000_0111;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_32_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000____0000_0000_0000_0000_0000_0000_0000_0000;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48_0)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000____0000_0000_0000_0000_0000_0000_0000_0011;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48_16_0)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0000____0000_0000_0000_0111_0000_0000_0000_0011;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NonNegatedValueFixture, NonNegatedValue_48_32_16_0)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_1111____0000_0000_0000_0111_0000_0000_0000_0011;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "NonNegatedValue_48_32_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }
}