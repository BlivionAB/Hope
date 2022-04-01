#include "CharacterLiteralFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(CharacterLiteralFixture, CharacterLiteral_Start)
    {
        testFunction("return '\\0';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Start",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_End)
    {
        testFunction("return '~';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_End",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_Digit_0)
    {
        testFunction("return '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Digit_0",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }

    TEST_F(CharacterLiteralFixture, CharacterLiteral_Digit_9)
    {
        testFunction("return '9';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Digit_9",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_Alphabet_Start)
    {
        testFunction("return 'A';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Alphabet_Start",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_Alphabet_End)
    {
        testFunction("return 'z';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Alphabet_End",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_Newline)
    {
        testFunction("return '\\n';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_Newline",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_CarriageReturn)
    {
        testFunction("return '\\r';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CarriageReturn",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_Tab)
    {
        testFunction("return '\\t';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_Tab",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }



    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_BackwardSlash)
    {
        testFunction("return '\\\\';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_BackwardSlash",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_Null)
    {
        testFunction("return '\\0';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_Null",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_CodePoint_Start)
    {
        testFunction("return '\\x00';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_Start",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_CodePoint_SingleCharacter)
    {
        testFunction("return '\\x1';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_SingleCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_CodePoint_End)
    {
        testFunction("return '\\xff';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_End",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequence_CodePoint_Error_ExceedingCharRange)
    {
        testFunction("return '\\xa00';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_Error_ExceedingCharRange",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_EscapeSequnce_UnsupportedCharacter)
    {
        testFunction("return '\\g';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequnce_UnsupportedCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_Error_MultiCharacter)
    {
        testFunction("return 'abc';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Error_MultiCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }

    TEST_F(CharacterLiteralFixture, CharacterLiteral_Error_UnsupportedCharacter_RandomUnicode)
    {
        testFunction("return '我';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Error_UnsupportedCharacter_RandomUnicode",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(CharacterLiteralFixture, CharacterLiteral_Error_UnsupportedCharacter_AsciiExtended)
    {
        testFunction("return 'å';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Error_UnsupportedCharacter_AsciiExtended",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}