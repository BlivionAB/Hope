#ifndef ELET_BITMASKIMMEDIATEFIXTURE_H
#define ELET_IMMEDIATEINSTRUCTIONENCODINGFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::instruction::output::test
{
    using namespace elet::domain::compiler::test;


    class NegatedValueFixture : public CompileFixture
    {
        std::filesystem::path
        basePath() override
        {
            return "src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/NegatedValue";
        }
    };
}


#endif //ELET_BITMASKIMMEDIATEFIXTURE_H
