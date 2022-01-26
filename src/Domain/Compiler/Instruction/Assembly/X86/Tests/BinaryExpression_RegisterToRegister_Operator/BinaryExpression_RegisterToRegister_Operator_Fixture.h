#ifndef ELET_BINARYEXPRESSION_REGISTERTOREGISTER_OPERATOR_FIXTURE_H
#define ELET_BINARYEXPRESSION_REGISTERTOREGISTER_OPERATOR_FIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::instruction::output::test
{
    using namespace elet::domain::compiler::test;


    class BinaryExpression_RegisterToRegister_Operator_Fixture : public CompileFixture
    {
        std::filesystem::path
        basePath() override
        {
            return "src/Domain/Compiler/Instruction/Assembly/X86/Tests/BinaryExpression_RegisterToRegister_Operator";
        }


        std::filesystem::path
        localTestPath() override
        {
            return "";
        }
    };
}


#endif //ELET_BINARYEXPRESSION_REGISTERTOREGISTER_OPERATOR_FIXTURE_H
