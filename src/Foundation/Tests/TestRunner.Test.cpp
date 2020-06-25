#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Foundation/TestRunner.h>
#include <Foundation/Tests/Mocks/PathMock.h>
#include "FoundationFixture.h"

using ::testing::Return;

TEST_F(FoundationFixture, Soem)
{
    EXPECT_CALL()
    TestRunner<BaselineTestProject<PathMock>> testRunner(project);
    testRunner.execute();
}