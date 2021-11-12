#ifndef ELET_X86FIXTURE_H
#define ELET_X86FIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::instruction::output::test
{
    using namespace elet::domain::compiler::test;


    class X86Fixture : public CompileFixture
    {
        std::filesystem::path
        basePath() override
        {
            return "src/Domain/Compiler/Instruction/Assembly/X86";
        }


        std::filesystem::path
        localTestPath() override
        {
            return "Tests";
        }
    };
}


#endif //ELET_X86FIXTURE_H
