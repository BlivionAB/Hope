#include "BitmaskImmediateFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize16)
    {
        testFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0001;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize8)
    {
        testFunction("return 0b0000_0001_0000_0001_0000_0001_0000_0001;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize4)
    {
        testFunction("return 0b0001_0001_0001_0001_0001_0001_0001_0001;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize4",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize2)
    {
        testFunction("return 0b0101_0101_0101_0101_0101_0101_0101_0101;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize2",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize1)
    {
        testFunction("return 0b1111_1111_1111_1111_1111_1111_1111_1111;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize1",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_HalfElement64)
    {
        testFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0000____0000_0000_0000_0011_0000_0000_0000_0011;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_HalfElement64",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }
}