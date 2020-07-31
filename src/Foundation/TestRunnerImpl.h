#include "TestRunner.h"


template<typename TBaselineTestProject, typename TTestSelection>
TestRunner<TBaselineTestProject, TTestSelection>::TestRunner(TBaselineTestProject& project):
    _project(project)
{

}

template<typename TBaselineTestProject, typename TTestSelection>
void
TestRunner<TBaselineTestProject, TTestSelection>::execute(const TTestSelection& testSelection)
{
    _project.validate();
    Path testsFolder = Path::resolve(_project.settings.rootFolder, "Tests");
    Path testFiles = Path::resolve(_project.settings.rootFolder, "Tests/**");
    Path currentFolder = Path::resolve(_project.settings.rootFolder, "Current");

    List<Path> files = Path::find(testFiles);
    for (const Path& file : files)
    {
        if (file.isFile())
        {
            for (const BaselineDocument<TTestSelection>& document : _project.settings.documents)
            {
                if (Path::extension(file) == document.extension)
                {
                    if (testSelection.test)
                    {
                        if (std::strcmp(Path::filename(file), testSelection.test) != 0)
                        {
                            continue;
                        }
                    }
                    Path pathRelativeToTestsFolder = Path::relativeTo(testsFolder, file);
                    Path outputFileCandidate = Path::resolve(currentFolder, pathRelativeToTestsFolder);
                    Path outputFolder = Path::folderOf(outputFileCandidate);
                    Utf8String outputStem = Path::stem(outputFileCandidate);
                    Path outputFile = Path::resolve(outputFolder, outputStem + ".x86_64.o");
                    document.executor.execute(file, outputFile, testSelection);
                }
            }
        }
    }
}
