#ifndef ELET_BINARYEXPRESSION_HEXADECIMALLITERALFIXTURE_H
#define ELET_BINARYEXPRESSION_HEXADECIMALLITERALFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class HexadecimalLiteralFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/IntegerLiteral/HexadecimalLiteral";
        }
    };
}


#endif //ELET_BINARYEXPRESSION_HEXADECIMALLITERALFIXTURE_H
