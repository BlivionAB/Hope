#include "HexadecimalLiteralFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_8Bits)
    {
        testFunction("return 0xff;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_8Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_16Bits)
    {
        testFunction("return 0xffff;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_16Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_32Bits)
    {
        testFunction("return 0xffff_ffff;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_32Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_64Bits)
    {
        testFunction("return 0xffff_ffff_ffff_ffff;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_64Bits",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_SInt64Overflow)
    {
        testFunction("return -0xffff_ffff_ffff_ffff;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_SInt64Overflow",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_Multiple_Underlines)
    {
        testFunction("return 0xffff______ffff_ffff_ffff;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_Multiple_Underlines",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(HexadecimalLiteralFixture, HexadecimalLiteral_Multiple_Error_CapitalHexCharacter)
    {
        testFunction("var a: s32 = 0xF;\nvar b: s32 = 0xE;\nvar c: s32 = 0xD;\nvar d: s32 = 0xC;\nvar e: s32 = 0xB;\nvar f: s32 = 0xA;");

        EXPECT_TRUE(testProject({
            .baselineName = "HexadecimalLiteral_Multiple_Error_CapitalHexCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

