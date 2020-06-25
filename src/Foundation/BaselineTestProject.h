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

struct BaselineDocument
{
    const char*
    extension;

    DocumentExecutor&
    executor;
};

struct ProjectSettings
{
    Path
    rootFolder;

    List<BaselineDocument>
    documents;
};


template<typename TPath>
class BaselineTestProject
{
public:

    ProjectSettings&
    settings;

    BaselineTestProject(ProjectSettings& projectSettings, TPath& path);

    void
    validate();

private:

    TPath&
    _path;
};


#include "BaselineTestProjectImpl.h"


#endif //ELET_BASELINETESTPROJECT_H
