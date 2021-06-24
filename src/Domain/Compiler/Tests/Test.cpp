#include <gtest/gtest.h>
#include "../Compiler.h"
#include "./Test.h"


namespace elet::domain::compiler::test
{


TEST_F(CompileFixture, DynamicLinkingWithCCode)
{
    project->setEntrySourceFile("main.l1",
        "domain Common::Console"
        "{ "
        ""
        "public:"
        ""
        "   fn WriteLine(text: char*): void"
        "   {"
        "       puts(text);"
        "   }"
        ""
        "private:"
        ""
        "   extern \"C\""
        "   {"
        "       fn puts(source: char*): int;"
        "       fn scanf(source: char*): int;"
        "   }"
        "}"
        ""
        "domain Common::MyApplication implements IConsoleApplication"
        "{"
        "   using Common::Console::{ WriteLine };"
        ""
        "   public:"
        ""
        "   fn OnProcessStart(): void"
        "   {"
        "       WriteLine(\"Hello World!\");"
        "   }"
        "}");

    EXPECT_TRUE(testProject(project, "some"));
}


}

