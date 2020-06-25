cmake_minimum_required(VERSION 3.16)
project(Elet)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party/googletest)
add_subdirectory(third_party/fmt)

add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS ON)

set(Boost_DEBUG on)
set(Boost_USE_MULTITHREADED TRUE)
find_package(Boost 1.67 COMPONENTS system filesystem regex REQUIRED)

include_directories(
    src
    third_party/googletest/googletest/include
    third_party/googletest/googlemock/include
    ${Boost_INCLUDE_DIR}
)

file(GLOB FOUNDATION_SRC ${CMAKE_SOURCE_DIR}/src/Foundation/*)
file(GLOB FOUNDATION_TEST ${CMAKE_SOURCE_DIR}/src/Foundation/Tests/* ${CMAKE_SOURCE_DIR}/src/Foundation/Tests/**/*)
file(GLOB DOMAIN_SRC ${CMAKE_SOURCE_DIR}/src/Domain/*)
file(GLOB DOMAIN_TEST ${CMAKE_SOURCE_DIR}/src/Domain/Tests/* ${CMAKE_SOURCE_DIR}/src/Domain/Tests/**/*)

add_executable(
    baselinetest
    baselinetest.cpp
    ${FOUNDATION_SRC}
    ${DOMAIN_SRC}
        src/Foundation/FileReader.cpp src/Foundation/FileReader.h src/Foundation/DocumentExecutor.cpp src/Foundation/DocumentExecutor.h)

add_executable(
    unittest
    unittest.cpp
    ${FOUNDATION_SRC}
    ${FOUNDATION_TEST}
    ${DOMAIN_SRC}
    ${DOMAIN_TEST}
        src/Foundation/Tests/TestRunner.Test.cpp src/Foundation/Tests/FoundationFixture.h)

set(LIBS fmt ${Boost_LIBRARIES})
set(TEST_LIBS gtest gmock)
target_link_libraries(unittest ${TEST_LIBS} ${LIBS})
target_link_libraries(baselinetest ${LIBS})