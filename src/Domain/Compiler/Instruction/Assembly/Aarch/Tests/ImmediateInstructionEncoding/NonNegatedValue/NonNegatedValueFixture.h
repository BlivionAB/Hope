#ifndef ELET_BITMASKIMMEDIATEFIXTURE_H
#define ELET_IMMEDIATEINSTRUCTIONENCODINGFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::instruction::output::test
{
    using namespace elet::domain::compiler::test;


    class NonNegatedValueFixture : public CompileFixture
    {
        std::filesystem::path
        basePath() override
        {
            return "src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/NonNegatedValue";
        }


        std::filesystem::path
        localTestPath() override
        {
            return "";
        }
    };
}


#endif //ELET_BITMASKIMMEDIATEFIXTURE_H
