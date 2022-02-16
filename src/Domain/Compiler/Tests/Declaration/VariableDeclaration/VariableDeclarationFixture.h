#ifndef ELET_DECLARATION_VARIABLEDECLARATION_H
#define ELET_DECLARATION_VARIABLEDECLARATION_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class VariableDeclarationFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Declaration/VariableDeclaration";
        }
    };
}


#endif //ELET_DECLARATION_VARIABLEDECLARATION_H
