#include "BaselineTestProject.h"
#include "Path.h"
template<typename TPath, typename TTestSelection>
BaselineTestProject<TPath, TTestSelection>::BaselineTestProject(ProjectSettings<TTestSelection>& settings, TPath& path):
    settings(settings),
    _path(path)
{

}


template<typename TPath, typename TTestSelection>
void
BaselineTestProject<TPath, TTestSelection>::validate()
{
    if (!_path.exists(settings.rootFolder))
    {
        throw NoRootFolderException();
    }
    if (!_path.exists(_path.resolve(settings.rootFolder, "Tests")))
    {
        throw NoTestsFolderException();
    }
    if (!_path.exists(_path.resolve(settings.rootFolder, "References")))
    {
        throw NoReferenceFolderException();
    }
}
