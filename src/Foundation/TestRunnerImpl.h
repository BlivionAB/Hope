#include "TestRunner.h"


template<typename TBaselineTestProject, typename TPath>
TestRunner<TBaselineTestProject, TPath>::TestRunner(TBaselineTestProject& project, TPath& path):
    _project(project),
    _path(path)
{

}

template<typename TBaselineTestProject, typename TPath>
void
TestRunner<TBaselineTestProject, TPath>::execute(const TestSelection& testSelection)
{
    _project.validate();
    List<Path> files = Path::find(Path::relative(_project.rootFolder, "Tests/**/*"));
    for (Path& file : files)
    {
        if (file.isFile())
        {
            for (const BaselineDocument& document : _project.settings.documents)
            {
                if (Path::extension(file) == document.extension)
                {
                    document.executor.execute(file);
                }
            }
        }
    }
}
