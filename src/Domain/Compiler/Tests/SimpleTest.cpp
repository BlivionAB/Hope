#include <gtest/gtest.h>
#include "../Compiler.h"
#include "./CompilerTestHarness.h"


namespace elet::domain::compiler::test
{


TEST_F(CompileFixture, ExternalFunction)
{
    project.setEntrySourceFile("main.l1",
        "domain Common::Console\n"
        "{\n"
        "\n"
        "public:\n"
        "\n"
        "   fn WriteLine(text: char*): void\n"
        "   {\n"
        "       puts(text);\n"
        "   }\n"
        "\n"
        "private:\n"
        "\n"
        "   extern \"C\"\n"
        "   {\n"
        "       fn puts(source: char*): int;\n"
        "   }\n"
        "}\n"
        "\n"
        "domain Common::MyApplication implements IConsoleApplication\n"
        "{\n"
        "   using Common::Console::{ WriteLine };\n"
        "\n"
        "public:\n"
        "\n"
        "   fn OnApplicationStart(): void\n"
        "   {\n"
        "       WriteLine(\"Hello World!\");\n"
        "   }\n"
        "}");

    EXPECT_TRUE(testProject({
        .baselineName = "external-function",
        .targets = {
            CompilationTarget::MachO_x86_64,
            CompilationTarget::MachO_Aarch64,
            CompilationTarget::StashIR
        },
    }));
}



TEST_F(CompileFixture, Return_BinaryExpression_Variable)
{
    testMainFunction(
        "var x = 1 + 2;\n"
        "var y = 1 + 2;\n"
        "return x + y;");

    EXPECT_TRUE(testProject({
        .baselineName = "return-binary_expression-variable",
        .targets = {
            CompilationTarget::StashIR,
            CompilationTarget::MachO_x86_64
        },
        .optimizationLevel = OptimizationLevel::_1,
    }));
}


TEST_F(CompileFixture, Return_BinaryExpresison_ImmediateValue)
{
    testMainFunction(
        "return 1 + 2;");

    EXPECT_TRUE(testProject({
        .baselineName = "return-binary_expression-immediate_value",
        .targets = {
            //            CompilationTarget::StashIR,
            CompilationTarget::MachO_x86_64
        },
        .optimizationLevel = OptimizationLevel::_1,
    }));
}

}

