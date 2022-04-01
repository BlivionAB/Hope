#ifndef ELET_BOOLEANLITERALFIXTURE_H
#define ELET_BINARYEXPRESSION_IMMEDIATEKINDFIXTURE_H

#include "Domain/Compiler/TestHarness/CompilerFixture.h"
#include <filesystem>


namespace elet::domain::compiler::test
{
    class BinaryExpression_BooleanToBooleanFixture :
        public CompileFixture,
        public ::testing::WithParamInterface<std::tuple<Utf8String, Utf8String, Utf8String, Utf8String>>
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/BooleanToBoolean";
        }
    };


    class BinaryExpression_BooleanToBoolean_OperatorPrecedenceFixture :
        public CompileFixture,
        public ::testing::WithParamInterface<std::tuple<Utf8String, Utf8String>>
    {
        std::filesystem::path
        localTestPath() override
        {
            return "Expression/BinaryExpression/BooleanToBoolean";
        }
    };
}


#endif //ELET_BOOLEANLITERALFIXTURE_H
