#ifndef ELET_BINARYEXPRESSION_VARIABLETOIMMEDIATE_H
#define ELET_BINARYEXPRESSION_VARIABLETOIMMEDIATE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class VariableToImmediateFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/VariableToImmediate";
        }
    };
}


#endif //ELET_BINARYEXPRESSION_VARIABLETOIMMEDIATE_H
