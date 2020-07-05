#include "BaselineTestProject.h"
#include "Path.h"
template<typename TPath>
BaselineTestProject<TPath>::BaselineTestProject(ProjectSettings& settings, TPath& path):
    settings(settings),
    _path(path)
{

}


template<typename TPath>
void
BaselineTestProject<TPath>::validate()
{
    if (!_path.exists(settings.rootFolder))
    {
        throw NoRootFolderException();
    }
    if (!_path.exists(_path.relative(settings.rootFolder, "Tests")))
    {
        throw NoTestsFolderException();
    }
    if (!_path.exists(_path.relative(settings.rootFolder, "References")))
    {
        throw NoReferenceFolderException();
    }
}
