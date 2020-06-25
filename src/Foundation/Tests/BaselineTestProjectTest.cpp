#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Foundation/BaselineTestProject.h>
#include <Foundation/Tests/Mocks/PathMock.h>
#include "FoundationFixture.h"

using ::testing::Return;

TEST_F(FoundationFixture, NoRootFolderException)
{
    EXPECT_CALL(pathMock, exists(cwd))
        .Times(1)
        .WillOnce(Return(false));

    BaselineTestProject<PathMock> project(projectSettings, pathMock);
    EXPECT_THROW(project.validate(), NoRootFolderException);
}

TEST_F(FoundationFixture, NoTestsFolderException)
{
    EXPECT_CALL(pathMock, exists(cwd))
            .Times(1)
            .WillOnce(Return(true));

    Path resultPath("cwd/Tests");
    EXPECT_CALL(pathMock, exists(resultPath))
            .Times(1)
            .WillOnce(Return(false));

    Path relativePath("cwd/Tests");
    Path path("Tests");
    EXPECT_CALL(pathMock, relativePathTo(cwd, path))
            .Times(1)
            .WillOnce(Return(relativePath));

    ProjectSettings projectSettings { cwd };
    BaselineTestProject<PathMock> project(projectSettings, pathMock);
    EXPECT_THROW(project.validate(), NoTestsFolderException);
}

TEST_F(FoundationFixture, NoReferenceFolderException)
{
    EXPECT_CALL(pathMock, exists(cwd))
            .Times(1)
            .WillOnce(Return(true));

    Path resultPath("cwd/Tests");
    EXPECT_CALL(pathMock, exists(resultPath))
        .Times(1)
        .WillOnce(Return(true));

    Path resultPath2("cwd/References");
    EXPECT_CALL(pathMock, exists(resultPath2))
        .Times(1)
        .WillOnce(Return(false));

    Path relativePath("cwd/Tests");
    Path path("Tests");
    EXPECT_CALL(pathMock, relativePathTo(cwd, path))
        .Times(1)
        .WillOnce(Return(relativePath));

    Path relativePath2("cwd/References");
    Path path2("References");
    EXPECT_CALL(pathMock, relativePathTo(cwd, path2))
        .Times(1)
        .WillOnce(Return(relativePath2));

    BaselineTestProject<PathMock> project(projectSettings, pathMock);
    EXPECT_THROW(project.validate(), NoReferenceFolderException);
}
