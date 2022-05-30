#include "VariableToImmediateFixture.h"


namespace elet::domain::compiler::test
{
    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableLeft)
    {
        testFunction(
            "var x: u32 = 1;\n"
            "return x + 1;", "u32");

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
            "var x: u32 = 1;\n"
            "return 1 + x;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableRight",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableRightSubType)
    {
        testFunction(
            "var x: u32 = 4294967295;\n"
            "return 1 + x;", "u64");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableRight_OverflowError",
            .targets = {
                CompilationTarget::StashIR
            }
        }));
    }


    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableRight_OverflowError)
    {
        testFunction(
            "var x: u32 = 4294967295;\n"
            "return 1 + x;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableRight_OverflowError",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableSuperType)
    {
        testFunction(
            "var x: u64 = 1;\n"
            "return 1 + x;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableSuperType",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableSubType)
    {
        testFunction(
            "var x: u8 = 1;\n"
            "return 1 + x;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableSubType",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }


    TEST_F(VariableToImmediateFixture, VariableToImmediate_VariableSuperTypeDownCastToEqualType)
    {
        testFunction(
            "var x: u64 = 1;\n"
            "return 1 + x as u32;", "u32");

        EXPECT_TRUE(testProject({
            .baselineName = "VariableToImmediate_VariableSuperTypeDownCastToEqualType",
            .targets = {
                CompilationTarget::StashIR
            },
        }));
    }
}

