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
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/Error/*
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
    ${CMAKE_SOURCE_DIR}/src/Domain/Compiler/TestHarness/TextDiff/Tests/*
    src/Domain/Compiler/Tests/Declaration/VariableDeclaration/VariableDeclaration_Test.cpp
    src/Domain/Compiler/Tests/Declaration/VariableDeclaration/VariableDeclarationFixture.h
    src/Domain/Compiler/Tests/Expression/CharacterLiteral/CharacterLiteral_Test.cpp
    src/Domain/Compiler/Tests/Expression/CharacterLiteral/CharacterLiteralFixture.h
    src/Domain/Compiler/Tests/Expression/Boolean/BooleanLiteral/BooleanLiteral_Test.cpp
    src/Domain/Compiler/Tests/Expression/Boolean/BooleanLiteral/BooleanLiteralFixture.h
    src/Domain/Compiler/Tests/Expression/BinaryExpression/BooleanToBoolean/BinaryExpression_BooleanToBoolean_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/BooleanToBoolean/BinaryExpression_BooleanToBooleanFixture.h
    src/Domain/Compiler/Tests/Expression/BinaryExpression/ImmediateKind/ImmediateKindFixture.h
    src/Domain/Compiler/Tests/Expression/BinaryExpression/ImmediateKind/BinaryExpression_ImmediateKind_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/ImmediateToImmediate/BinaryExpression_ImmediateToImmediate_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/ImmediateToImmediate/BinaryExpression_ImmediateToImmediateFixture.h
    src/Domain/Compiler/Tests/Expression/BinaryExpression/VariableToImmediate/VariableToImmediate_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/VariableToImmediate/VariableToImmediateFixture.h
    src/Domain/Compiler/Tests/Expression/BinaryExpression/VariableToVariable/VariableToVariable_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/VariableToVariable/VariableToVariableFixture.h
    src/Domain/Compiler/Tests/Expression/IntegerLiteral/HexadecimalLiteral/HexadecimalLiteral_Test.cpp
    src/Domain/Compiler/Tests/Expression/IntegerLiteral/DecimalLiteral/DecimalLiteral_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/OperatorKind/OperatorKind_Test.cpp
    src/Domain/Compiler/Tests/Expression/BinaryExpression/OperatorKind/OperatorKindFixture.h

    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/BitmaskImmediate/BitmaskImmediate_Test.cpp
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/BitmaskImmediate/BitmaskImmediateFixture.h
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/NonNegatedValue/NonNegatedValue_Test.cpp
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/NonNegatedValue/NonNegatedValueFixture.h
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/NegatedValue/NegatedValue_Test.cpp
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/ImmediateInstructionEncoding/NegatedValue/NegatedValueFixture.h
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/OperatorKind/OperatorKind_Test.cpp
    src/Domain/Compiler/Instruction/Assembly/Aarch/Tests/OperatorKind/OperatorKindFixture.h

    src/Domain/Compiler/Instruction/Assembly/X86/Tests/OperatorKind/OperatorKind_Test.cpp
)

#message(STATUS ${DOMAIN_TEST})

add_executable(
    unittest
    src/unittest.cpp
    ${FOUNDATION_SRC}
    ${FOUNDATION_TEST}
    ${DOMAIN_SRC}
    ${DOMAIN_TEST} )

set(TEST_LIBS gtest gmock)
target_link_libraries(unittest ${TEST_LIBS})
