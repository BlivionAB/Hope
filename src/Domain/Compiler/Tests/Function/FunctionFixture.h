#ifndef ELET_FUNCTIONFIXTURE_H
#define ELET_FUNCTIONFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{

    class FunctionFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Function";
        }
    };

}

#endif //ELET_EXPRESSIONFIXTURE_H
