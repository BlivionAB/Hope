#include "./BooleanLiteralFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BooleanLiteralFixture, BooleanLiteral_True)
    {
        testFunction("var a = true;", "void");

        EXPECT_TRUE(testProject({
            .baselineName = "BooleanLiteral_True",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BooleanLiteralFixture, BooleanLiteral_False)
    {
        testFunction("var a = false;", "void");

        EXPECT_TRUE(testProject({
            .baselineName = "BooleanLiteral_False",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BooleanLiteralFixture, BooleanLiteral_AssignNonBooleanPlaceholder)
    {
        testFunction("var a: u32 = false;", "void");

        EXPECT_TRUE(testProject({
            .baselineName = "BooleanLiteral_AssignNonBooleanPlaceholder",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

