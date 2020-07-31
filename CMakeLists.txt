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
    src/baselinetest.cpp
    ${FOUNDATION_SRC}
    ${DOMAIN_SRC}
        src/Domain/Compiler.cpp src/Domain/Compiler.h src/Tests/EletSourceDocumentExecutor.cpp src/Tests/EletSourceDocumentExecutor.h src/Domain/Parser.Error.h src/Domain/Instruction/Scanner.cpp src/Domain/Instruction/Scanner.h src/Foundation/BaseScanner.cpp src/Foundation/BaseScanner.h src/Domain/Instruction/Parser.cpp src/Domain/Instruction/Parser.h src/Domain/Instruction/Syntax.h src/Domain/Instruction/Parser.Error.h src/Domain/Instruction/Instruction.h src/Domain/Instruction/Transformer.cpp src/Domain/Instruction/Transformer.h src/Domain/Instruction/Assembly/Baseline/BaselineWriter.cpp src/Domain/Instruction/Assembly/Baseline/BaselineWriter.h src/Domain/Instruction/TextWriter.cpp src/Domain/Instruction/TextWriter.h src/Domain/Binder.cpp src/Domain/Binder.h src/Domain/Instruction/ObjectFileWriter/MachOFileWriter.h src/Domain/Instruction/AssemblyWriter.cpp src/Domain/Instruction/AssemblyWriter.h src/Domain/Instruction/Assembly/x86/X86_64Writer.cpp src/Domain/Instruction/Assembly/x86/X86_64Writer.h src/Domain/Instruction/Assembly/AssemblyWriterInterface.h src/Domain/Instruction/ObjectFileWriter/MachOFileWriter.cpp src/Domain/Instruction/ObjectFileWriter.cpp src/Domain/Instruction/ObjectFileWriter.h src/Domain/Instruction/ObjectFileWriter/ObjectFileWriterInterface.h)

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