#ifndef ELET_OPERATORKINDFIXTURE_H
#define ELET_OPERATORKINDFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::instruction::output::test
{
    using namespace elet::domain::compiler::test;


    class X86OperatorKindFixture : public CompileFixture
    {
        std::filesystem::path
        basePath() override
        {
            return "src/Domain/Compiler/Instruction/Assembly/X86/Tests/OperatorKind";
        }
    };
}


#endif //ELET_OPERATORKINDFIXTURE_H
