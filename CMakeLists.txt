cmake_minimum_required(VERSION 3.15)
project(Elet)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party/googletest)

include_directories(
    src
    third_party/googletest/googletest/include
    third_party/googletest/googlemock/include
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
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/**/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/Expression/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Tests/Function/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/TestHarness/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/TestHarness/X86/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/TestHarness/Aarch/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/TestHarness/TextDiff/*
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/TestHarness/TextDiff/Tests/*)

message(STATUS ${DOMAIN_SRC})

add_executable(
    unittest
    src/unittest.cpp
    ${FOUNDATION_SRC}
    ${FOUNDATION_TEST}
    ${DOMAIN_SRC}
    ${DOMAIN_TEST} src/Domain/Compiler/Tests/Expression/IntegerLiteral_Decimal_Test.cpp)

set(TEST_LIBS gtest gmock)
target_link_libraries(unittest ${TEST_LIBS})
