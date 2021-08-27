#ifndef ELET_EXTERNALFUNCTION_H
#define ELET_EXTERNALFUNCTION_H


#include <gmock/gmock.h>
#include <fstream>
#include <optional>
#include "CompilerBaselineParser.h"
#include <Foundation/File.h>
#include <filesystem>
#include "./TextDiff/MyersDiff.h"
#include "./TextDiff/DiffPrinter.h"
#include "../../../unittest.h"



namespace elet::domain::compiler::test
{

namespace fs = std::filesystem;


enum class OptimizationLevel
{
    None,
    _1,
    _2,
};


struct TestProjectOptions
{

    std::string
    baselineName;

    AssemblyTarget
    assemblyTarget;

    ObjectFileTarget
    objectFileTarget;

    std::optional<bool>
    writeExecutable;

    std::optional<OptimizationLevel>
    optimizationLevel;
};


class FileStreamReaderMock : public FileStreamReader
{

public:

    FileStreamReaderMock(std::map<fs::path, Utf8String>& filePathToFileSourceMap):
        _filePathToFileSourceMap(filePathToFileSourceMap)
    {

    }

    void
    openFile(const fs::path filepath) override
    {
        auto result = _filePathToFileSourceMap.find(filepath);
        if (result != _filePathToFileSourceMap.end())
        {
            _currentIterSpan = new IterationSpan(result->second.beginChar(), result->second.endChar());
        }
    }


    size_t
    readChunk(char* buffer, size_t size) override
    {
        size_t i = 0;
        while (_currentIterSpan->start != _currentIterSpan->end && i != size)
        {
            buffer[i] = *_currentIterSpan->start;
            ++_currentIterSpan->start;
            ++i;
        }
        return i;
    }


    size_t
    getFileSize() override
    {
        return _currentIterSpan->end.getValue() - _currentIterSpan->start.getValue();
    }

private:

    struct IterationSpan
    {
        Utf8String::CharIterator
        start;

        Utf8String::CharIterator
        end;

        IterationSpan(Utf8String::CharIterator start, Utf8String::CharIterator end):
            start(start),
            end(end)
        { }
    };

    IterationSpan*
    _currentIterSpan;

    const char*
    _cursor;

    std::map<fs::path, Utf8String>&
    _filePathToFileSourceMap;

    struct SourceCursor
    {
        Utf8String::Iterator
        source;

        const char*
        cursor;
    };


    std::map<int, IterationSpan*>
    _fdToSourceIteratorMap;
};


class TestProject
{

public:

    TestProject()
    {
        fileReader = new FileStreamReaderMock(_filePathToFileSourceMap);
    }


    void
    setEntrySourceFile(fs::path filepath, Utf8String filesource)
    {
        _entryFile = filepath;
        setSourceFile(filepath, filesource);
    }


    void
    setSourceFile(fs::path filepath, Utf8String filesource)
    {
        _filePathToFileSourceMap[filepath] = filesource;
    }


    fs::path
    getEntryFile() const
    {
        return _entryFile;
    }

    FileStreamReaderMock*
    fileReader;

private:

    std::map<fs::path, Utf8String>
    _filePathToFileSourceMap;

    fs::path
    _entryFile;
};


class CompileFixture : public ::testing::Test
{
protected:

    void SetUp()
    {

    }

    fs::path
    currentPath = "src/Domain/Compiler/Tests/__Baselines__";

    TestProject
    project;

    testing::AssertionResult
    testProject(TestProjectOptions options)
    {
        Compiler compiler(*project.fileReader, options.assemblyTarget, options.objectFileTarget);
        List<uint8_t>& output2 = compiler.getOutput();

        compiler.startWorkers();
        compiler.addFile(project.getEntryFile());
        compiler.endWorkers();
        List<uint8_t>& output = compiler.getOutput();
        switch (options.assemblyTarget)
        {
            case AssemblyTarget::x86_64:
                return checkTextSegmentBaselines<x86::X86AssemblyParser, x86::X86AssemblyPrinter, x86::Instruction>(
                    options,
                    output);
            case AssemblyTarget::Aarch64:
                return checkTextSegmentBaselines<Aarch64AssemblyParser, Aarch64AssemblyPrinter, OneOfInstruction>(
                    options,
                    output);
        }

    }

    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TInstruction>
    testing::AssertionResult
    checkTextSegmentBaselines(TestProjectOptions& options, List<uint8_t>& output)
    {
        CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TInstruction> baselineParser(output, options.assemblyTarget);
        if (options.writeExecutable)
        {
            writeOutput(output, options.baselineName + ".o");
        }

        baselineParser.parse();
        Utf8String textSection = baselineParser.serializeTextSegment();
        auto textSectionResult = checkBaseline(textSection, options.baselineName + ".basm");
        if (!textSectionResult)
        {
            return textSectionResult;
        }
        return testing::AssertionSuccess();
    }


    testing::AssertionResult
    checkBaseline(Utf8String& result, fs::path baselineName)
    {
        DiffPrinter printer;
        MyersDiff differ;
        fs::path basm = fs::current_path() / ".." / currentPath / baselineName;
        fs::path baselineFolder = basm.parent_path();
        if (!fs::exists(baselineFolder))
        {
            fs::create_directories(baselineFolder);
        }
        bool isDiffing = false;
        Utf8String baseline("");
        if (fs::exists(basm))
        {
            baseline = File::read(basm);
        }

        auto diffs = differ.diffText(baseline, result, isDiffing);
        if (isDiffing)
        {
            if (std::getenv("UPDATE_BASELINES") != nullptr || std::getenv("U") != nullptr)
            {
                File::write(basm, result);
                return testing::AssertionSuccess();
            }
            else
            {
                return testing::AssertionFailure() << printer.print(diffs, true);
            }
        }
        File::write(basm, result);
        return testing::AssertionSuccess();
    }

    void
    writeOutput(const List<uint8_t>& output, fs::path file) const
    {
        std::ofstream fileHandle;
        const char* path = (fs::current_path() / file).string().c_str();
        fileHandle.open(path, std::ios_base::binary);
        for (int i = 0; i < output.size(); ++i)
        {
            fileHandle.write(reinterpret_cast<char*>(&output[i]), 1);
        }
        std::cout << path << std::endl;
        fileHandle.close();
    }
};


}

#endif //ELET_EXTERNALFUNCTION_H
