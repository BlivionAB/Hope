#include "NegatedValueFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_0)
    {
        testFunction("return 0b1111_1111_1111_1111_1111_1111_1111_1111;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_16)
    {
        testFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32)
    {
        testFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___1111_1111_1111_1111_1111_1111_1111_1111;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32_0)
    {
        testFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___1111_1111_1111_1111_0000_0000_0000_0001;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32_16)
    {
        testFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___0000_0000_0000_0001_1111_1111_1111_1111;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32_16_0_OutfOfBounds)
    {
        testFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___0000_0000_0000_0001_0000_0000_0000_0011;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32_16_0_OutfOfBounds",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48)
    {
        testFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111___1111_1111_1111_1111_1111_1111_1111_1111;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48_0)
    {
        testFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111___1111_1111_1111_1111_0000_0000_0000_0001;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48_16_0_OutOfBounds)
    {
        testFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111___0000_0000_0000_0011_0000_0000_0000_0001;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48_16_0_OutOfBounds",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48_32_16_0_OutOfBounds)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0111___0000_0000_0000_0011_0000_0000_0000_0001;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48_32_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }
}