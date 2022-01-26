#include "./BinaryExpression_ImmediateKindFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_IntegerLeft_IntegerRight)
    {
        testMainFunction("return 1 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_IntegerLeft_IntegerRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_CharLeft_IntegerRight)
    {
        testMainFunction("return '0' + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_CharLeft_IntegerRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_IntegerLeft_CharRight)
    {
        testMainFunction("return 2 + '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_IntegerLeft_CharRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_CharLeft_CharRight)
    {
        testMainFunction("return '2' + '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_CharLeft_CharRight",
            .targets = {
                CompilationTarget::StashIR
            },
            .acceptBaselines = true
        }));
    }
}

