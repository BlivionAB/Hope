#include "./BinaryExpression_BooleanToBooleanFixture.h"
#include <gtest/gtest.h>

namespace elet::domain::compiler::test
{
    TEST_P(BinaryExpression_BooleanToBooleanFixture, BooleanToBoolean)
    {
        Utf8String binop = std::get<0>(GetParam());
        Utf8String binopName = std::get<1>(GetParam());
        Utf8String op1 = std::get<2>(GetParam());
        Utf8String op2 = std::get<3>(GetParam());
        testFunction("return " + op1 + " " + binop + " " + op2 + ";", "bool");
        Utf8String baselineName = binopName + "_" + op1.capitalize() + "To" + op2.capitalize();

        EXPECT_TRUE(testProject({
            .baselineName = baselineName.toString(),
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }

    INSTANTIATE_TEST_SUITE_P(
        BinaryExpression_BooleanToBoolean_And,
        BinaryExpression_BooleanToBooleanFixture,
        ::testing::Values(
            std::make_tuple("&&", "And", "true", "true"),
            std::make_tuple("&&", "And", "true", "false"),
            std::make_tuple("&&", "And", "false", "true"),
            std::make_tuple("&&", "And", "false", "false")
        ));


    INSTANTIATE_TEST_SUITE_P(
        BinaryExpression_BooleanToBoolean_Or,
        BinaryExpression_BooleanToBooleanFixture,
        ::testing::Values(
            std::make_tuple("||", "Or", "true", "true"),
            std::make_tuple("||", "Or", "true", "false"),
            std::make_tuple("||", "Or", "false", "true"),
            std::make_tuple("||", "Or", "false", "false")
        ));

    TEST_P(BinaryExpression_BooleanToBoolean_OperatorPrecedenceFixture, OperatorPrecedence)
    {
        Utf8String expr = std::get<0>(GetParam());
        Utf8String baselineName = std::get<1>(GetParam());
        testFunction(expr, "bool");

        EXPECT_TRUE(testProject({
            .baselineName = baselineName.toString(),
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    INSTANTIATE_TEST_SUITE_P(
        BinaryExpression_BooleanToBoolean_OperatorPrecedence,
        BinaryExpression_BooleanToBoolean_OperatorPrecedenceFixture,
        ::testing::Values(
            std::make_tuple("return false || false && false;", "OperatorPrecedence_FalseOrFalseAndFalse"),
            std::make_tuple("return false || false && true;", "OperatorPrecedence_FalseOrFalseAndTrue"),
            std::make_tuple("return false || true && false;", "OperatorPrecedence_FalseOrTrueAndFalse"),
            std::make_tuple("return false || true && true;", "OperatorPrecedence_FalseOrTrueAndTrue"),
            std::make_tuple("return true || false && false;", "OperatorPrecedence_TrueOrFalseAndFalse"),
            std::make_tuple("return true || false && true;", "OperatorPrecedence_TrueOrFalseAndTrue"),
            std::make_tuple("return true || true && false;", "OperatorPrecedence_TrueOrTrueAndFalse"),
            std::make_tuple("return true || true && true;", "OperatorPrecedence_TrueOrTrueAndTrue")
        ));
}

