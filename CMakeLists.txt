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

file(GLOB DOMAIN_SRC
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Syntax/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/ObjectFileWriter/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/Assembly/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/Assembly/Aarch/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/Assembly/x86/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/Assembly/x86/X86Parser/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Instruction/Assembly/x86/OpCode/*)

file(GLOB DOMAIN_TEST
    ${CMAKE_SOURCE_DIR}/src/Domain/Tests/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Tests/**/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/X86/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/Aarch/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/TextDiff/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/TextDiff/Tests/*)

message(STATUS ${DOMAIN_SRC})

add_executable(
    unittest
    src/unittest.cpp
    ${FOUNDATION_SRC}
    ${FOUNDATION_TEST}
    ${DOMAIN_SRC}
    ${DOMAIN_TEST} src/Domain/Compiler/Tests/Aarch/Aarch64BaselinePrinter.cpp src/Domain/Compiler/Tests/Aarch/Aarch64BaselinePrinter.h)

set(LIBS fmt ${Boost_LIBRARIES})
set(TEST_LIBS gtest gmock)
target_link_libraries(unittest ${TEST_LIBS} ${LIBS})
