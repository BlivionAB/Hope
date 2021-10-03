#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{


TEST_F(ExpressionFixture, IntegerLiteral_Hex_8Bits)
{
    testMainFunction("return 0xff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_Hex_8Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


TEST_F(ExpressionFixture, IntegerLiteral_Hex_16Bits)
{
    testMainFunction("return 0xffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_Hex_16Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


TEST_F(ExpressionFixture, IntegerLiteral_Hex_32Bits)
{
    testMainFunction("return 0xffffffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_Hex_32Bits",
        .targets = {
            CompilationTarget::StashIR
        },
        .updateBaselines = true,
    }));
}

TEST_F(ExpressionFixture, IntegerLiteral_Hex_64Bits)
{
    testMainFunction("return 0xffff_ffff_ffff_ffff;");

    EXPECT_TRUE(testProject({
        .baselineName = "IntegerLiteral_Hex_64Bits",
        .targets = {
            CompilationTarget::StashIR
        },
    }));
}


}

