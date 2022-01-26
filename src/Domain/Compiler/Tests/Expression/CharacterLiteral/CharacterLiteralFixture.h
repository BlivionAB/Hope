#ifndef ELET_BINARYEXPRESSION_IMMEDIATEKINDFIXTURE_H
#define ELET_BINARYEXPRESSION_IMMEDIATEKINDFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class CharacterLiteralFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/CharacterLiteral";
        }
    };
}


#endif //ELET_BINARYEXPRESSION_IMMEDIATEKINDFIXTURE_H
