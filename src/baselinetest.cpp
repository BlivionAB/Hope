#include <iostream>
#include <Domain/CommandLine/BaselineTestCommand.h>
#include <Foundation/Printer.h>
#include <Tests/EletSourceDocumentExecutor.h>

int
main(int argc, char *argv[])
{
    Printer printer;
    Path path;
    EletSourceDocumentExecutor executor;
    BaselineDocument eletDocument
    {
        ".l1",
        executor
    };
    ProjectSettings settings
    {
        Path::relative(Path::cwd(), "src/Domain/Tests/Baselines"),
        { eletDocument }
    };
    BaselineTestProject project(settings, path);
    TestRunner<BaselineTestProject<Path>> testRunner(project);
    BaselineTestCommand<
        Printer,
        TestRunner<BaselineTestProject<Path>>
    > command(printer, testRunner);
    return command.execute(argc, argv);
}
