#ifndef ELET_FOUNDATIONFIXTURE_H
#define ELET_FOUNDATIONFIXTURE_H

#include <gtest/gtest.h>
#include <Foundation/FilePath.h>
#include <Foundation/Tests/Mocks/PathMock.h>

class FoundationFixture : public ::testing::Test
{
public:
    FilePath
    cwd = FilePath("cwd");

    PathMock
    pathMock;

    ProjectSettings
    projectSettings = { cwd };

    BaselineTestProject<PathMock>
    project = BaselineTestProject<PathMock>(projectSettings, pathMock);
};

#endif // ELET_FOUNDATIONFIXTURE_H
