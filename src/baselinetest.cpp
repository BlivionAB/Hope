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
    BaselineDocument<BaselineTestSelection> eletDocument
    {
        ".l1",
        executor
    };
    ProjectSettings<BaselineTestSelection> settings
    {
            Path::resolve(Path::cwd(), "src/Domain/Tests/Baselines"),
        { eletDocument }
    };
    BaselineTestProject<Path, BaselineTestSelection> project(settings, path);
    TestRunner<BaselineTestProject<Path, BaselineTestSelection>, BaselineTestSelection> testRunner(project);
    BaselineTestCommand<
        Printer,
        TestRunner<BaselineTestProject<Path, BaselineTestSelection>, BaselineTestSelection>
    > command(printer, testRunner);
    return command.execute(argc, argv);
}
