#include "VariableToImmediateFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableLeft)
    {
        testFunction(
            "var x = 1 + 2;\n"
            "return x + 1;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableLeft",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableRight)
    {
        testFunction(
            "var x = 1 + 2;\n"
            "return 1 + x;");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

