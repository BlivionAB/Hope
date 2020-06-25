#include "BaselineTestProject.h"

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
    if (!_path.exists(_path.relativePathTo(settings.rootFolder, "Tests")))
    {
        throw NoTestsFolderException();
    }
    if (!_path.exists(_path.relativePathTo(settings.rootFolder, "References")))
    {
        throw NoReferenceFolderException();
    }
}
