#ifndef ELET_BOOLEANLITERALFIXTURE_H
#define ELET_BOOLEANLITERALFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class BooleanLiteralFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/Boolean/BooleanLiteral";
        }
    };
}


#endif //ELET_BOOLEANLITERALFIXTURE_H
