#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Domain/CommandLine/BaselineTestCommand.h>
#include <Foundation/Tests/Mocks/PrinterMock.h>
#include <Foundation/Tests/Mocks/TestRunnerMock.h>
#include <Foundation/Tests/Mocks/PathMock.h>

using ::testing::_;
using ::testing::Return;

TEST(RootCommand, Help)
{
    PrinterMock printerMock;
    PathMock pathMock;
    ProjectSettings projectSettings { "cwd" };
    BaselineTestProject<PathMock> project(projectSettings, pathMock);
    TestRunnerMock<BaselineTestProject<PathMock>> testRunnerMock(project);
    BaselineTestCommand<
        PrinterMock,
        TestRunnerMock<BaselineTestProject<PathMock>>
    > command(printerMock, testRunnerMock);

    EXPECT_CALL(printerMock, print(HELP_TEXT))
        .Times(1);

    char* argv[2];
    argv[0] = strdup("test");
    argv[1] = strdup("--help");
    int exitCode = command.execute(2, argv);
    EXPECT_EQ(exitCode, 0);
}

TEST(RunCommand, ShouldBeAbleToRunTests)
{
    PrinterMock printerMock;
    PathMock pathMock;
    ProjectSettings projectSettings { "cwd" };
    BaselineTestProject<PathMock> project(projectSettings, pathMock);
    TestRunnerMock<BaselineTestProject<PathMock>> testRunnerMock(project);
    BaselineTestCommand<
            PrinterMock,
            TestRunnerMock<BaselineTestProject<PathMock>>
    > command(printerMock, testRunnerMock);

    EXPECT_CALL(testRunnerMock, execute(_))
            .Times(1);

    char* argv[2];
    argv[0] = strdup("test");
    argv[1] = strdup("run");
    int exitCode = command.execute(2, argv);
    EXPECT_EQ(exitCode, 0);
}
