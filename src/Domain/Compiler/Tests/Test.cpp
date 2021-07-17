#include <gtest/gtest.h>
#include "../Compiler.h"
#include "./Test.h"


namespace elet::domain::compiler::test
{


TEST_F(CompileFixture, DynamicLinkingWithCCode)
{
    project->setEntrySourceFile("main.l1",
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
        "   fn OnProcessStart(): void\n"
        "   {\n"
        "       WriteLine(\"Hello World!\");\n"
        "   }\n"
        "}");

//    EXPECT_TRUE(testProject(project, AssemblyTarget::x86_64, ObjectFileTarget::MachO, "macho-x86_64"));
    EXPECT_TRUE(testProject(project, AssemblyTarget::AArch64, ObjectFileTarget::MachO, "macho-aarch64"));
}


}

