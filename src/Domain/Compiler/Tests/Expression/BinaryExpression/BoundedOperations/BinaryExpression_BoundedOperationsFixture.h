#ifndef ELET_BOOLEANLITERALFIXTURE_H
#define ELET_BINARYEXPRESSION_IMMEDIATEKINDFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class BinaryExpression_BoundedOperationsFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/BoundedOperations";
        }
    };
}


#endif //ELET_BOOLEANLITERALFIXTURE_H
