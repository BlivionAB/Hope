#ifndef ELET_TEST_H
#define ELET_TEST_H


#include <gmock/gmock.h>
#include "CompilerBaselineParser.h"
#include <Foundation/FilePath.h>
#include <Foundation/File.h>
#include "./TextDiff/MyersDiff.h"
#include "./TextDiff/DiffPrinter.h"
#include "../../../unittest.h"


namespace elet::domain::compiler::test
{


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
        project = new TestProject();
    }

    Utf8String
    currentPath = "src/Domain/Compiler/Tests/__Baselines__";

    TestProject*
    project;

    testing::AssertionResult
    testProject(const TestProject* project, const Utf8String baselineName)
    {
        Compiler compiler(*project->fileReader, AssemblyTarget::x86_64, ObjectFileTarget::MachO);
        compiler.startWorkers();
        compiler.addFile(project->getEntryFile());
        compiler.endWorkers();
        auto output = compiler.getOutput();
        CompilerBaselineParser baselineParser(output);
        Utf8String result = baselineParser.write();
        DiffPrinter printer;
        MyersDiff differ;
        bool isDiffing = false;
        FilePath basm = FilePath::cwd() / ".." / currentPath / (baselineName + ".basm");
        FilePath baselineFolder = FilePath::folderOf(basm);
        if (!FilePath::exists(baselineFolder))
        {
            File::createDirectory(baselineFolder);
        }
        if (FilePath::exists(basm))
        {
            auto baseline = File::read(basm);
            auto diffs = differ.diffText(result, baseline, isDiffing);
            if (isDiffing)
            {
                if (std::getenv("UPDATE_BASELINES") != nullptr || std::getenv("U") != nullptr)
                {
                    File::write(basm, result);
                    return testing::AssertionSuccess();
                }
                else
                {
                    return testing::AssertionFailure() << printer.print(diffs);
                }
            }
            return testing::AssertionSuccess();
        }
        File::write(basm, result);
        return testing::AssertionSuccess();
    }
};


}

#endif //ELET_TEST_H
