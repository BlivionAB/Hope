#ifndef ELET_EXPRESSIONFIXTURE_H
#define ELET_EXPRESSIONFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{

    class ExpressionFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression";
        }
    };

}

#endif //ELET_EXPRESSIONFIXTURE_H
