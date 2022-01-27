#ifndef ELET_BINARYEXPRESSION_VARIABLETOVARIABLE_H
#define ELET_BINARYEXPRESSION_VARIABLETOVARIABLE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class VariableToVariableFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/VariableToVariable";
        }
    };
}


#endif //ELET_BINARYEXPRESSION_VARIABLETOIMMEDIATE_H
