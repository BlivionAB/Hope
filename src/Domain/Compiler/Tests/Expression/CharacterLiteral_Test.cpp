#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(ExpressionFixture, CharacterLiteral_Start)
    {
        testMainFunction("return '\\0';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Start",
            .targets = {
                CompilationTarget::StashIR
            },
            .acceptBaselines = true,
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_End)
    {
        testMainFunction("return '~';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_End",
            .targets = {
                CompilationTarget::StashIR
            },
            .acceptBaselines = true,
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_Digit_0)
    {
        testMainFunction("return '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Digit_0",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }

    TEST_F(ExpressionFixture, CharacterLiteral_Digit_9)
    {
        testMainFunction("return '9';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Digit_9",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_Alphabet_Start)
    {
        testMainFunction("return 'A';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Alphabet_Start",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_Alphabet_End)
    {
        testMainFunction("return 'z';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Alphabet_End",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_Newline)
    {
        testMainFunction("return '\\n';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_Newline",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_CarriageReturn)
    {
        testMainFunction("return '\\r';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CarriageReturn",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_Tab)
    {
        testMainFunction("return '\\t';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_Tab",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }



    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_BackwardSlash)
    {
        testMainFunction("return '\\\\';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_BackwardSlash",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_Null)
    {
        testMainFunction("return '\\0';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_Null",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_CodePoint_Start)
    {
        testMainFunction("return '\\x00';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_Start",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_CodePoint_SingleCharacter)
    {
        testMainFunction("return '\\x1';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_SingleCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_CodePoint_End)
    {
        testMainFunction("return '\\xff';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_End",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequence_CodePoint_Error_ExceedingCharRange)
    {
        testMainFunction("return '\\xa00';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequence_CodePoint_Error_ExceedingCharRange",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_EscapeSequnce_UnsupportedCharacter)
    {
        testMainFunction("return '\\g';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_EscapeSequnce_UnsupportedCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_Error_MultiCharacter)
    {
        testMainFunction("return 'abc';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Error_MultiCharacter",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }

    TEST_F(ExpressionFixture, CharacterLiteral_Error_UnsupportedCharacter_RandomUnicode)
    {
        testMainFunction("return '我';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Error_UnsupportedCharacter_RandomUnicode",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(ExpressionFixture, CharacterLiteral_Error_UnsupportedCharacter_AsciiExtended)
    {
        testMainFunction("return 'å';");

        EXPECT_TRUE(testProject({
            .baselineName = "CharacterLiteral_Error_UnsupportedCharacter_AsciiExtended",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}