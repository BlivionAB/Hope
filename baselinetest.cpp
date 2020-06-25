#include <iostream>
#include <Domain/CommandLine/BaselineTestCommand.h>
#include <Foundation/Printer.h>

int
main(int argc, char *argv[])
{
    Printer printer;
    Path path;
    ProjectSettings settings {Path::relative(Path::cwd(), "src/Tests") };
    BaselineTestProject<Path> project(settings, path);
    TestRunner<BaselineTestProject<Path>> testRunner(project);
    BaselineTestCommand<
        Printer,
        TestRunner<BaselineTestProject<Path>>
    > command(printer, testRunner);
    return command.execute(argc, argv);
}
