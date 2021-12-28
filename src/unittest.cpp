#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>


int
main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
};