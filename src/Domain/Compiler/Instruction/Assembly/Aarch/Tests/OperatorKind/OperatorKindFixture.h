#ifndef ELET_AARCH_OPERATORKIND_FIXTURE_H
#define ELET_AARCH_OPERATORKIND_FIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::instruction::output::test
{
    using namespace elet::domain::compiler::test;


    class AarchOperatorKindFixture : public CompileFixture
    {
        std::filesystem::path
        basePath() override
        {
            return "src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/OperatorKind";
        }
    };
}


#endif //ELET_AARCH_OPERATORKIND_FIXTURE_H
