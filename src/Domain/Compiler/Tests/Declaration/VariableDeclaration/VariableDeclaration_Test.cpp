#include "./VariableDeclarationFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(VariableDeclarationFixture, Coercion_32ToS8)
    {
        testMainFunction("var a: s32 = 1 + 2;\n"
                         "return a;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_32ToS8",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_32ToS16)
    {
        testMainFunction("var a: s32 = 1 + 2;\n"
                         "return a;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_32ToS16",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_32ToS32)
    {
        testMainFunction("var a: s32 = 1 + 2;\n"
                         "return a;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_32ToS32",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_32ToS64)
    {
        testMainFunction("var a: s32 = 1 + 2;\n"
                         "return a;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_32ToS64",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_64ToS8)
    {
        testMainFunction("var a: s64 = 1 + 2;\n"
                         "return a;", "s8");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_64ToS8",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_64ToS16)
    {
        testMainFunction("var a: s64 = 1 + 2;\n"
                         "return a;", "s16");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_64ToS16",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_64ToS32)
    {
        testMainFunction("var a: s64 = 1 + 2;\n"
                         "return a;", "s32");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_64ToS32",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableDeclarationFixture, Coercion_64ToS64)
    {
        testMainFunction("var a: s64 = 1 + 2;\n"
                         "return a;", "s64");

        EXPECT_TRUE(testProject({
            .baselineName = "Coercion_64ToS64",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}