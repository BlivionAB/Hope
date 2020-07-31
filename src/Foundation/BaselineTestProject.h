#ifndef ELET_BASELINETESTPROJECT_H
#define ELET_BASELINETESTPROJECT_H

#include <exception>
#include "Path.h"
#include "List.h"
#include "DocumentExecutor.h"

struct NoRootFolderException: std::exception
{

};


struct NoTestsFolderException: std::exception
{

};


struct NoReferenceFolderException: std::exception
{

};


template<typename TTestSelection>
struct BaselineDocument
{
    const char*
    extension;

    DocumentExecutor<TTestSelection>&
    executor;
};


template<typename TTestSelection>
struct ProjectSettings
{
    Path
    rootFolder;

    std::vector<BaselineDocument<TTestSelection>>
    documents;
};


template<typename TPath, typename TTestSelection>
class BaselineTestProject
{
public:

    ProjectSettings<TTestSelection>&
    settings;

    BaselineTestProject(ProjectSettings<TTestSelection>& projectSettings, TPath& path);

    void
    validate();

private:

    TPath&
    _path;
};


#include "BaselineTestProjectImpl.h"


#endif //ELET_BASELINETESTPROJECT_H
