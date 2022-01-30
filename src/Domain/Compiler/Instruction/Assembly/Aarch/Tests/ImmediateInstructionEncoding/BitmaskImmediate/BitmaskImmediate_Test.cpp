#include "BitmaskImmediateFixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize16)
    {
        testMainFunction("return 0b0000_0000_0000_0001_0000_0000_0000_0001;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize16",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize8)
    {
        testMainFunction("return 0b0000_0001_0000_0001_0000_0001_0000_0001;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize8",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize4)
    {
        testMainFunction("return 0b0001_0001_0001_0001_0001_0001_0001_0001;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize4",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize2)
    {
        testMainFunction("return 0b0101_0101_0101_0101_0101_0101_0101_0101;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize2",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_ElementSize1)
    {
        testMainFunction("return 0b1111_1111_1111_1111_1111_1111_1111_1111;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_ElementSize1",
            .targets = {
                CompilationTarget::MachO_Aarch64
            },
        }));
    }


    TEST_F(BitmaskImmediateFixture, ImmediateInstructionEncoding_BitmaskImmediate_HalfElement64)
    {
        testMainFunction("return 0b0000_0000_0000_0000_0000_0000_0000_0000____0000_0000_0000_0011_0000_0000_0000_0011;");

        EXPECT_TRUE(testProject({
            .baselineName = "ImmediateInstructionEncoding_BitmaskImmediate_HalfElement64",
            .targets = {
                CompilationTarget::MachO_Aarch64,
            },
        }));
    }
}