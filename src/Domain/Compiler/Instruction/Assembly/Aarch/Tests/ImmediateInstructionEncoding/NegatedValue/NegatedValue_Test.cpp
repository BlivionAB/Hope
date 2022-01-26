#include "NegatedValueFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_0)
    {
        testMainFunction("return 0b1111_1111_1111_1111_1111_1111_1111_1111;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_16)
    {
        testMainFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32)
    {
        testMainFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___1111_1111_1111_1111_1111_1111_1111_1111;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32_0)
    {
        testMainFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___1111_1111_1111_1111_0000_0000_0000_0001;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32_16)
    {
        testMainFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___0000_0000_0000_0001_1111_1111_1111_1111;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32_16",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_32_16_0_OutfOfBounds)
    {
        testMainFunction("return 0b1111_1111_1111_1111_0000_0000_0000_0000___0000_0000_0000_0001_0000_0000_0000_0011;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_32_16_0_OutfOfBounds",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48)
    {
        testMainFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111___1111_1111_1111_1111_1111_1111_1111_1111;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48_0)
    {
        testMainFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111___1111_1111_1111_1111_0000_0000_0000_0001;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48_16_0_OutOfBounds)
    {
        testMainFunction("return 0b0000_0000_0000_0000_1111_1111_1111_1111___0000_0000_0000_0011_0000_0000_0000_0001;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48_16_0_OutOfBounds",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }


    TEST_F(NegatedValueFixture, ImmediateInstructionEncoding_NegatedValue_48_32_16_0_OutOfBounds)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0111___0000_0000_0000_0011_0000_0000_0000_0001;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_NegatedValue_48_32_16_0",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }
}