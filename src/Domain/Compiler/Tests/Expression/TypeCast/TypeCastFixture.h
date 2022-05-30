#ifndef ELET_TYPECAST_FIXTURE_H
#define ELET_TYPECAST_FIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class TypeCastFixture : public CompileFixture
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/TypeCast";
        }
    };
}


#endif // ELET_TYPECAST_FIXTURE_H
