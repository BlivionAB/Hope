#include "HexadecimalLiteralFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_8Bits)
    {
        testFunction("return 0xff;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_8Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_16Bits)
    {
        testFunction("return 0xffff;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_16Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_32Bits)
    {
        testFunction("return 0xffff_ffff;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_32Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_64Bits)
    {
        testFunction("return 0xffff_ffff_ffff_ffff;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_64Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_SInt64Overflow)
    {
        testFunction("return -0xffff_ffff_ffff_ffff;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_SInt64Overflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_Multiple_Underlines)
    {
        testFunction("return 0xffff______ffff_ffff_ffff;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_Multiple_Underlines",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_Multiple_Error_CapitalHexCharacter)
    {
        testFunction("var a: int = 0xF;\nvar b: int = 0xE;\nvar c: int = 0xD;\nvar d: int = 0xC;\nvar e: int = 0xB;\nvar f: int = 0xA;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_Multiple_Error_CapitalHexCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

