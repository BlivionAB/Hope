#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{


TEST_F(ExpressionFixture, IntegerLiteral_HexadecimalLiteral_8Bits)
{
    testMainFunction("return 0xff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_HexadecimalLiteral_8Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


TEST_F(ExpressionFixture, IntegerLiteral_HexadecimalLiteral_16Bits)
{
    testMainFunction("return 0xffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_HexadecimalLiteral_16Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


TEST_F(ExpressionFixture, IntegerLiteral_HexadecimalLiteral_32Bits)
{
    testMainFunction("return 0xffff_ffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_HexadecimalLiteral_32Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


TEST_F(ExpressionFixture, IntegerLiteral_HexadecimalLiteral_64Bits)
{
    testMainFunction("return 0xffff_ffff_ffff_ffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_HexadecimalLiteral_64Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


TEST_F(ExpressionFixture, IntegerLiteral_HexadecimalLiteral_SInt64Overflow)
{
    testMainFunction("return -0xffff_ffff_ffff_ffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_HexadecimalLiteral_SInt64Overflow",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


}

