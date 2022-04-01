#include "./BinaryExpression_ImmediateKindFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_IntegerLeft_IntegerRight)
    {
        testFunction("return 1 + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_IntegerLeft_IntegerRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_CharLeft_IntegerRight)
    {
        testFunction("return '0' + 2;");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_CharLeft_IntegerRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_IntegerLeft_CharRight)
    {
        testFunction("return 2 + '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_IntegerLeft_CharRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(BinaryExpression_ImmediateKindFixture, BinaryExpression_ImmediateKind_CharLeft_CharRight)
    {
        testFunction("return '2' + '0';");

        EXPECT_TRUE(testProject({
            .baselineName = "BinaryExpression_ImmediateKind_CharLeft_CharRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

