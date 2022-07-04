#ifndef ELET_OPERATORKINDFIXTURE_H
#define ELET_OPERATORKINDFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class OperatorKindFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/OperatorKind";
        }
    };
}


#endif //ELET_OPERATORKINDFIXTURE_H
