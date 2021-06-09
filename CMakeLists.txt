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
file(GLOB DOMAIN_SRC ${CMAKE_SOURCE_DIR}/src/Domain/**)
file(GLOB DOMAIN_TEST ${CMAKE_SOURCE_DIR}/src/Domain/Tests/* ${CMAKE_SOURCE_DIR}/src/Domain/Tests/**/*)

add_executable(
    baselinetest
    src/baselinetest.cpp
    ${FOUNDATION_SRC}
    ${DOMAIN_SRC}
    src/Domain/Compiler/Compiler.cpp
    src/Domain/Compiler/Compiler.h
    src/Tests/EletSourceDocumentExecutor.cpp
    src/Tests/EletSourceDocumentExecutor.h
    src/Domain/Compiler/Parser.cpp
    src/Domain/Compiler/Scanner.cpp
    src/Domain/Compiler/Instruction/Scanner.cpp
    src/Domain/Compiler/Instruction/Scanner.h
    src/Foundation/BaseScanner.cpp
    src/Foundation/BaseScanner.h
    src/Domain/Compiler/Instruction/Parser.cpp
    src/Domain/Compiler/Instruction/Parser.h
    src/Domain/Compiler/Instruction/Syntax.h
    src/Domain/Compiler/Instruction/Parser.Error.h
        src/Domain/Compiler/Instruction/Instruction.h
    src/Domain/Compiler/Instruction/Transformer.cpp
    src/Domain/Compiler/Instruction/Transformer.h
    src/Domain/Compiler/Instruction/Assembly/Baseline/BaselineWriter.cpp
    src/Domain/Compiler/Instruction/Assembly/Baseline/BaselineWriter.h
    src/Domain/Compiler/Instruction/TextWriter.cpp
    src/Domain/Compiler/Instruction/TextWriter.h
    src/Domain/Compiler/Binder.cpp
    src/Domain/Compiler/Binder.h
    src/Domain/Compiler/Instruction/ObjectFileWriter/MachOFileWriter.h
    src/Domain/Compiler/Instruction/AssemblyWriter.cpp
    src/Domain/Compiler/Instruction/AssemblyWriter.h
    src/Domain/Compiler/Instruction/Assembly/x86/X86_64Writer.cpp
    src/Domain/Compiler/Instruction/Assembly/x86/X86_64Writer.h
    src/Domain/Compiler/Instruction/Assembly/AssemblyWriterInterface.h
    src/Domain/Compiler/Instruction/ObjectFileWriter/MachOFileWriter.cpp
    src/Domain/Compiler/Instruction/ObjectFileWriter.cpp
    src/Domain/Compiler/Instruction/ObjectFileWriter.h
    src/Domain/Compiler/Instruction/ObjectFileWriter/ObjectFileWriterInterface.h
    src/Domain/Compiler/Instruction/Assembly/x86/OpCode/Mov.h
    src/Domain/Compiler/Instruction/Assembly/x86/OpCode/GeneralOpCodes.h
    src/Domain/Compiler/Instruction/Assembly/x86/OpCode/AddressForm32.h
    src/Domain/Compiler/Checker.cpp src/Domain/Compiler/Checker.h
    src/Domain/Compiler/Syntax/Syntax.Declarations.h
    src/Domain/Compiler/Syntax/Syntax.Kind.h
    src/Domain/Compiler/Syntax/Syntax.Labels.h
    src/Domain/Compiler/Syntax/Syntax.Block.h
    src/Domain/Compiler/Syntax/Syntax.Type.h src/Domain/Compiler/Instruction/Instruction.Constant.h src/Domain/Compiler/Instruction/ObjectFileWriter/BaselineObjectFileWriter.cpp src/Domain/Compiler/Instruction/ObjectFileWriter/BaselineObjectFileWriter.h src/Domain/Compiler/Instruction/Assembly/x86/X86Parser/X86BaselinePrinter.cpp src/Domain/Compiler/Instruction/Assembly/x86/X86Parser/X86BaselinePrinter.h src/Domain/Compiler/Instruction/Assembly/x86/X86Parser/X86Types.h src/Domain/Compiler/Instruction/Assembly/x86/X86Parser/X86BaselineParser.cpp src/Domain/Compiler/Instruction/Assembly/x86/X86Parser/X86BaselineParser.h src/Domain/Compiler/Instruction/Assembly/AssemblyWriterInterface.cpp src/Domain/Compiler/Instruction/ObjectFileWriter/DyldInfoWriter.cpp src/Domain/Compiler/Instruction/ObjectFileWriter/DyldInfoWriter.h src/Domain/Compiler/Instruction/ByteWriter.cpp src/Domain/Compiler/Instruction/ByteWriter.h src/Domain/Compiler/Instruction/ObjectFileWriter/ExportInfoWriter.cpp src/Domain/Compiler/Instruction/ObjectFileWriter/ExportInfoWriter.h src/Domain/Compiler/Instruction/ObjectFileWriter/ExportTrieTree.cpp src/Domain/Compiler/Instruction/ObjectFileWriter/ExportTrieTree.h)

add_executable(
    unittest
    src/unittest.cpp
    ${FOUNDATION_SRC}
    ${FOUNDATION_TEST}
    ${DOMAIN_SRC}
    ${DOMAIN_TEST}
)

set(LIBS fmt ${Boost_LIBRARIES})
set(TEST_LIBS gtest gmock)
target_link_libraries(unittest ${TEST_LIBS} ${LIBS})
target_link_libraries(baselinetest ${LIBS})