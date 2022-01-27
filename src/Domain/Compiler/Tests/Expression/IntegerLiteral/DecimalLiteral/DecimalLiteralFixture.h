#ifndef ELET_BINARYEXPRESSION_DECIMALLITERALFIXTURE_H
#define ELET_BINARYEXPRESSION_DECIMALLITERALFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class DecimalLiteralFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/IntegerLiteral/DecimalLiteral";
        }
    };
}


#endif //ELET_BINARYEXPRESSION_DECIMALLITERALFIXTURE_H
