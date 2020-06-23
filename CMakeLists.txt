cmake_minimum_required(VERSION 3.16)
project(Elet)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)

add_executable(
    test
    test.cpp
    src/Foundation/Utf8String.h
    src/Foundation/Utf8String.cpp
    src/Foundation/Buffer.h
    src/Foundation/Buffer.cpp
    src/Foundation/HashTableMap.h
    src/Foundation/HashTableMapImpl.h
    src/Foundation/HashKeyValueNode.h
    src/Foundation/HashKeyValueNodeImpl.h
    src/Foundation/FirstCharacterHash.h
    src/Foundation/FirstCharacterHashImpl.h
    src/Foundation/Utf8StringView.h
    src/Foundation/Utf8StringView.cpp
    src/Foundation/Memory/MemoryPool.h
    src/Foundation/Memory/MemoryPool.cpp
    src/Foundation/Memory/TextSpan.h
    src/Foundation/Memory/TextSpan.cpp
    src/Foundation/Memory/Utf8Span.h
    src/Foundation/Memory/Utf8Span.cpp
    src/Foundation/Memory/Utf8SpanIterator.h
    src/Foundation/Memory/Utf8SpanIterator.cpp
    src/Foundation/Memory/TextSpanCharacterIterator.h
    src/Foundation/Memory/TextSpanCharacterIterator.cpp
    src/Foundation/Memory/TextSpanChunkIterator.h
    src/Foundation/Memory/TextSpanChunkIterator.cpp
    src/Domain/Scanner.cpp
    src/Domain/Scanner.h
    src/Domain/Parser.cpp
    src/Domain/Parser.h
    src/Domain/Syntax.h
    src/Domain/Exceptions.h
    src/Foundation/CommandLineImpl.h
    src/Foundation/CommandLine.h
    src/Domain/CommandLine/Command.cpp
    src/Domain/CommandLine/Command.h
    src/Domain/CommandLine/Command.Test.cpp
    src/Foundation/Printer.cpp
    src/Foundation/Printer.h
)