#include "X86Fixture.h"


namespace elet::domain::compiler::instruction::output::test
{
    TEST_F(X86Fixture, X86_Operator_Add)
    {
        testMainFunction(
            "var x = 1 + 2;\n"
            "var y = 1 + 2;\n"
            "return x + y;");

        EXPECT_TRUE(testProject({
            .baselineName = "X86_Operator_Add",
            .targets = {
                CompilationTarget::MachO_x86_64
            }
        }));
    }
}