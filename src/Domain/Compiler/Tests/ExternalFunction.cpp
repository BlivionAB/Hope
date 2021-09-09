#include <gtest/gtest.h>
#include "../Compiler.h"
#include "./ExternalFunction.h"


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
        "       fn scanf(source: char*): int;\n"
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
        .baselineName = "macho-x86_64",
        .assemblyTarget = AssemblyTarget::x86_64,
        .objectFileTarget = ObjectFileTarget::MachO,
    }));

    EXPECT_TRUE(testProject({
        .baselineName = "macho-aarch64",
        .assemblyTarget = AssemblyTarget::Aarch64,
        .objectFileTarget = ObjectFileTarget::MachO,
    }));
}



TEST_F(CompileFixture, CommonSubExpressionElimination)
{
    project.setEntrySourceFile("main.l1",
        "domain Common::MyApplication implements IConsoleApplication\n"
        "{\n"
        "\n"
        "public:\n"
        "\n"
        "    fn OnApplicationStart(): void\n"
        "    {\n"
        "        var x = 1 + 2;\n"
        "        var y = 1 + 2;\n"
        "        return x + y;\n"
        "    }\n"
        "}");

    EXPECT_TRUE(testProject({
        .baselineName = "common-subexpression-elimination-x86_64",
        .assemblyTarget = AssemblyTarget::StashIR,
        .optimizationLevel = OptimizationLevel::_1,
    }));
}

}

