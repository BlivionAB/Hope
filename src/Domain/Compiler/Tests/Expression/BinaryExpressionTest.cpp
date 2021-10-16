#include "ExpressionFixture.h"


namespace elet::domain::compiler::test
{


TEST_F(ExpressionFixture, Return_BinaryExpression_Variable)
{
    testMainFunction(
        "var x = 1 + 2;\n"
        "var y = 1 + 2;\n"
        "return x + y;");

    EXPECT_TRUE(testProject({
        .baselineName = "Return_BinaryExpression_Variable",
        .targets = {
            CompilationTarget::StashIR,
            CompilationTarget::MachO_x86_64
        },
        .acceptBaselines = true,
    }));
}


TEST_F(ExpressionFixture, Return_BinaryExpresison_ImmediateValue_LowerThan16Bit)
{
    testMainFunction("return 1 + 2;");

    EXPECT_TRUE(testProject({
        .baselineName = "Return_BinaryExpresison_ImmediateValue_LowerThan16Bit",
        .targets = {
            CompilationTarget::StashIR,
            CompilationTarget::MachO_x86_64,
            CompilationTarget::MachO_Aarch64
        },
    }));
}


TEST_F(ExpressionFixture, Return_BinaryExpresison_ImmediateValue_LargerThan16Bit_BitmaskImmediate_2x32)
{
    testMainFunction("return 65536 + 1;");

    EXPECT_TRUE(testProject({
        .baselineName = "Return_BinaryExpresison_ImmediateValue_LargerThan16Bit_BitmaskImmediate2_x32",
        .targets = {
            CompilationTarget::MachO_Aarch64
        },
    }));
}


TEST_F(ExpressionFixture, Return_BinaryExpresison_ImmediateValue_LargerThan16Bit_MovzMovk)
{
    testMainFunction("return 65536 + 2;");

    EXPECT_TRUE(testProject({
        .baselineName = "Return_BinaryExpresison_ImmediateValue_LargerThan16Bit_MovzMovk",
        .targets = {
            CompilationTarget::MachO_Aarch64
        },
    }));
}


TEST_F(ExpressionFixture, Return_BinaryExpresison_ImmediateValue_LargerThan16Bit_NonBitmaskImmediate)
{
    testMainFunction("return 65536 + 2;");

    EXPECT_TRUE(testProject({
        .baselineName = "return-binary_expression-immediate_value",
        .targets = {
            CompilationTarget::MachO_Aarch64
        },
    }));
}


}

