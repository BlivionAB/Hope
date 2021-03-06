#include "FunctionFixture.h"


namespace elet::domain::compiler::test
{


TEST_F(FunctionFixture, ExternalFunction)
{
    project.setEntrySourceFile("main.hs",
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
        "       fn puts(source: char*): s32;\n"
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
        .baselineName = "ExternalFunction",
        .targets =
        {
//            CompilationTarget::Pe32_x86_64,
            CompilationTarget::MachO_x86_64,
            CompilationTarget::MachO_Aarch64,
            CompilationTarget::StashIR
        },
        .generateTestFunction = false
    }));
}


}

