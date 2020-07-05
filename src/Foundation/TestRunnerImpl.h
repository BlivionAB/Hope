#include "TestRunner.h"


template<typename TBaselineTestProject>
TestRunner<TBaselineTestProject>::TestRunner(TBaselineTestProject& project):
    _project(project)
{

}

template<typename TBaselineTestProject>
void
TestRunner<TBaselineTestProject>::execute(const TestSelection& testSelection)
{
    _project.validate();
    Path relat = Path::relative(_project.settings.rootFolder, "Tests/**");
    List<Path> files = Path::find(relat);
    for (Path& file : files)
    {
        std::cout << file.toString() << std::endl;
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
