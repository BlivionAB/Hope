#ifndef ELET_TEST_H
#define ELET_TEST_H


#include <gmock/gmock.h>
#include <fstream>
#include <optional>
#include "CompilerBaselineParser.h"
#include <Foundation/FilePath.h>
#include <Foundation/File.h>
#include "./TextDiff/MyersDiff.h"
#include "./TextDiff/DiffPrinter.h"
#include "../../../unittest.h"



namespace elet::domain::compiler::test
{


struct TestProjectOptions
{
    ObjectFileTarget
    objectFileTarget;

    AssemblyTarget
    assemblyTarget;

    Utf8String
    baselineName;

    std::optional<bool>
    assertStubs;
};

class FileReaderMock : public FileReader
{

public:

    FileReaderMock(std::map<Utf8String, Utf8String>& filePathToFileSourceMap):
        _filePathToFileSourceMap(filePathToFileSourceMap)
    {

    }

    int
    openFile(const FilePath& filepath) override
    {
        auto result = _filePathToFileSourceMap.find(filepath.toString());
        if (result != _filePathToFileSourceMap.end())
        {
            auto iterSpan = new IterationSpan(result->second.beginChar(), result->second.endChar());
            _fdToSourceIteratorMap[assignedFd] = iterSpan;
            return assignedFd++;
        }
        return -1;
    }


    size_t
    readChunk(int fd, char* buffer, size_t size) override
    {
        auto iterationSpan = _fdToSourceIteratorMap[fd];
        size_t i = 0;
        while (iterationSpan->start != iterationSpan->end && i != size)
        {
            buffer[i] = *iterationSpan->start;
            ++iterationSpan->start;
            ++i;
        }
        return i;
    }


    size_t
    getFileSize(int fd) override
    {
        auto iterationSpan = _fdToSourceIteratorMap[fd];
        return iterationSpan->end.getValue() - iterationSpan->start.getValue();
    }

private:

    size_t
    assignedFd = 1;


    const char*
    _cursor;

    std::map<Utf8String, Utf8String>&
    _filePathToFileSourceMap;

    struct SourceCursor
    {
        Utf8String::Iterator
        source;

        const char*
        cursor;
    };


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
    std::map<int, IterationSpan*>
    _fdToSourceIteratorMap;
};


class TestProject
{

public:

    TestProject()
    {
        fileReader = new FileReaderMock(_filePathToFileSourceMap);
    }

    void
    setEntrySourceFile(FilePath filepath, Utf8String filesource)
    {
        _entryFile = filepath;
        setSourceFile(filepath, filesource);
    }

    void
    setSourceFile(FilePath filepath, Utf8String filesource)
    {
        _filePathToFileSourceMap[filepath.toString()] = filesource;
    }

    FilePath
    getEntryFile() const
    {
        return _entryFile;
    }

    FileReaderMock*
    fileReader;

private:

    std::map<Utf8String, Utf8String>
    _filePathToFileSourceMap;

    FilePath
    _entryFile;
};


class CompileFixture : public ::testing::Test
{
protected:

    void SetUp()
    {

    }

    Utf8String
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
                return baselineTest<x86::X86AssemblyParser, x86::X86AssemblyPrinter, x86::Instruction>(options, output);
            case AssemblyTarget::Aarch64:
                return baselineTest<Aarch64AssemblyParser, Aarch64AssemblyPrinter, OneOfInstruction>(options, output);
        }

    }

    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TInstruction>
    testing::AssertionResult
    baselineTest(TestProjectOptions& options, List<uint8_t>& output)
    {
        CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TInstruction> baselineParser(output, options.assemblyTarget);
        Utf8String result = baselineParser.serializeTextSection();
        DiffPrinter printer;
        MyersDiff differ;
        bool isDiffing = false;
        FilePath basm = FilePath::cwd() / ".." / currentPath / (options.baselineName + ".basm");
        FilePath baselineFolder = FilePath::folderOf(basm);

        writeOutput(output);

        if (!FilePath::exists(baselineFolder))
        {
            File::createDirectory(baselineFolder);
        }
        Utf8String baseline("");
        if (FilePath::exists(basm))
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
    writeOutput(const List<uint8_t>& output) const
    {
        std::ofstream file;
        const char* path = FilePath::resolve(FilePath::cwd(), "test.o").toString().toString();
        file.open(path, std::ios_base::binary);
        for (int i = 0; i < output.size(); ++i)
        {
            file.write(reinterpret_cast<char*>(&output[i]), 1);
        }
        std::cout << path << std::endl;
        file.close();
    }
};


}

#endif //ELET_TEST_H
