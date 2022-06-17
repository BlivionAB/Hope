#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include "Domain/Compiler/CompilerTypes.h"


namespace elet::domain::compiler::test
{
    class BinaryExpression_OverflowErrorFixture :
        public CompileFixture,
        public ::testing::WithParamInterface<std::tuple<Utf8String, Utf8String, Utf8String>>
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/OverflowError";
        }
    };


    TEST_P(BinaryExpression_OverflowErrorFixture, Add_Overflow)
    {
        Utf8String expression = std::get<0>(GetParam());
        Utf8String type = std::get<1>(GetParam());
        Utf8String testName = std::get<2>(GetParam());
        testFunction("return " + expression + ";", type);
        Utf8String baselineName = testName;

        EXPECT_TRUE(testProject({
            .baselineName = baselineName.toString(),
            .targets = {
                CompilationTarget::StashIR
            },
            .acceptBaselines = true
        }));
    }

    Utf8String S32MaxIntString = std::to_string(static_cast<int64_t>(SignedIntegerLimit::S32Max));
    Utf8String U64MaxIntString = std::to_string(static_cast<uint64_t>(IntegerLimit::U64Max));

    INSTANTIATE_TEST_SUITE_P(
        BinaryExpression_OverflowError,
        BinaryExpression_OverflowErrorFixture,
        ::testing::Values(
            std::make_tuple(U64MaxIntString + " + 1", "u64", "U64MaxOverflow_Begin"),
            std::make_tuple("-1 + " + U64MaxIntString + " * 2", "u64", "U64MaxOverflow_End")
        ));
}

