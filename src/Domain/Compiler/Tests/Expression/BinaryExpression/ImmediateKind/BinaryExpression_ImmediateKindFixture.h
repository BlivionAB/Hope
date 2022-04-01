#ifndef ELET_BOOLEANLITERALFIXTURE_H
#define ELET_BINARYEXPRESSION_IMMEDIATEKINDFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class BinaryExpression_ImmediateKindFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/ImmediateKind";
        }
    };
}


#endif //ELET_BOOLEANLITERALFIXTURE_H
