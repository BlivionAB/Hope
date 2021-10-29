#ifndef ELET_COMPILERFIXTURE_H
#define ELET_COMPILERFIXTURE_H


#include <gmock/gmock.h>
#include <fstream>
#include <optional>
#include "CompilerBaselineParser.h"
#include <Foundation/File.h>
#include <filesystem>
#include "Domain/Compiler/TestHarness/TextDiff/MyersDiff.h"
#include "Domain/Compiler/TestHarness/TextDiff/DiffPrinter.h"
#include "unittest.h"
#include "StashIrPrinter.h"



namespace elet::domain::compiler::test
{

namespace fs = std::filesystem;


enum class OptimizationLevel
{
    None,
    _1,
    _2,
};


enum class CompilationTarget
{
    MachO_x86_64,
    MachO_Aarch64,
    StashIR,
};


struct TestProjectOptions
{

    std::string
    baselineName;

    List<CompilationTarget>
    targets;

    std::optional<bool>
    printResolutionTypes;

    std::optional<bool>
    writeExecutable;

    std::optional<bool>
    acceptBaselines;

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

    virtual
    std::filesystem::path
    localTestPath()
    {
        throw std::runtime_error("You have not specified test path.");
    }


    void SetUp()
    {

    }

    fs::path
    testFolderPath = "src/Domain/Compiler/Tests/";

    TestProject
    project;

    StashIRPrinter
    stashIRPrinter;


    void
    testMainFunction(Utf8String source)
    {
        project.setEntrySourceFile("main.hs",
            Utf8String() + "domain Common::MyApplication implements IConsoleApplication\n"
            "{\n"
            "\n"
            "public:\n"
            "\n"
            "    fn OnApplicationStart(): void\n"
            "    {\n"
            + source +
            "    }\n"
            "}");
    }

    CompilerOptions
    getCompilerOptionsFromCompilationTarget(const CompilationTarget compilationTarget)
    {
        AssemblyTarget assemblyTarget;
        ObjectFileTarget objectFileTarget;
        switch (compilationTarget)
        {
            case CompilationTarget::MachO_Aarch64:
                assemblyTarget = AssemblyTarget::Aarch64;
                objectFileTarget = ObjectFileTarget::MachO;
                break;
            case CompilationTarget::MachO_x86_64:
                assemblyTarget = AssemblyTarget::x86_64;
                objectFileTarget = ObjectFileTarget::MachO;
                break;
            case CompilationTarget::StashIR:
                assemblyTarget = AssemblyTarget::StashIR;
                objectFileTarget = ObjectFileTarget::StashIR;
                break;
            default:
                throw std::runtime_error("Unknown compilation target.");
        }
        return CompilerOptions(assemblyTarget, objectFileTarget);
    }


    testing::AssertionResult
    testProject(TestProjectOptions options)
    {
        testing::AssertionResult result(true);
        for (const auto target : options.targets)
        {
            compileTarget(target, options, result);
        }
        return result;
    }


    void
    compileTarget(const CompilationTarget target, const TestProjectOptions& options, testing::AssertionResult& result)
    {
        CompilerOptions compilerOptions = getCompilerOptionsFromCompilationTarget(target);
        Compiler compiler(*project.fileReader, compilerOptions);

        compiler.startWorkers();
        compiler.compileFile(project.getEntryFile());
        compiler.endWorkers();

        Utf8String compileErrors = compiler.printCompileErrors();
        if (compileErrors.size() != 0)
        {
            checkBaseline(compileErrors, options, compilerOptions, result, "error");
        }
        else
        {
            if (compilerOptions.assemblyTarget == AssemblyTarget::StashIR)
            {
                std::queue<output::FunctionRoutine*> output = compiler.getStashIR();
                checkStashIRBaseline(output, options, compilerOptions, result);
            }
            else
            {
                List<uint8_t>& output = compiler.getOutput();
                switch (compilerOptions.assemblyTarget)
                {
                    case AssemblyTarget::x86_64:
                        checkTextSegmentBaselines<x86::X86AssemblyParser, x86::X86AssemblyPrinter, x86::Instruction>(options, compilerOptions, output, result);
                        break;
                    case AssemblyTarget::Aarch64:
                        checkTextSegmentBaselines<Aarch64AssemblyParser, Aarch64AssemblyPrinter, OneOfInstruction>(options, compilerOptions, output,result);
                        break;
                }
            }
        }
    }

    std::string
    getArchitecture(const CompilerOptions& compilerOptions)
    {
        if (compilerOptions.assemblyTarget == AssemblyTarget::StashIR)
        {
            return "StashIr";
        }
        switch (compilerOptions.objectFileTarget)
        {
            case ObjectFileTarget::MachO:
            {
                std::string result = "MachO";
                switch (compilerOptions.assemblyTarget)
                {
                    case AssemblyTarget::Aarch64:
                        return result + "-Aarch64";
                    case AssemblyTarget::x86_64:
                        return result + "-X86_64";
                }
                break;
            }
        }
        throw std::runtime_error("Unknown target in getArchitecture.");
    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TInstruction>
    void
    checkTextSegmentBaselines(const TestProjectOptions& options, const CompilerOptions& compilerOptions, List<uint8_t>& output, testing::AssertionResult& result)
    {
        CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TInstruction> baselineParser(output, compilerOptions.assemblyTarget);
        if (options.writeExecutable)
        {
            writeOutput(output, options.baselineName + ".o");
        }

        baselineParser.parse();
        Utf8String textSection = baselineParser.serializeTextSegment();
        checkBaseline(textSection, options, compilerOptions, result, "basm");
    }


    void
    checkBaseline(Utf8String& result, const TestProjectOptions& options, const CompilerOptions& compilerOptions, testing::AssertionResult& testResult, std::string suffix)
    {
        DiffPrinter printer;
        MyersDiff differ;
        fs::path basm = fs::current_path() / ".." / testFolderPath / localTestPath() / "__Baselines__" / (options.baselineName + "-" + getArchitecture(compilerOptions) + "." + suffix);
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
            if (options.acceptBaselines)
            {
                File::write(basm, result);
            }
            else
            {
                testing::AssertionResult failure(false);
                failure << std::endl;
                failure << testResult.message();
                failure << std::endl;
                failure << basm.filename().c_str() << std::endl;
                failure << "==============================" << std::endl;
                failure << printer.print(diffs, true);
                testResult = failure;
            }
            return;
        }
        File::write(basm, result);
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


    void
    checkStashIRBaseline(std::queue<output::FunctionRoutine*>& output, const TestProjectOptions& options, const CompilerOptions& compilerOptions, testing::AssertionResult& result)
    {
        Utf8String baselineOutput = stashIRPrinter.writeFunctionRoutines(output);
        checkBaseline(baselineOutput, options, compilerOptions, result, "basm");
    }
};


}

#endif //ELET_COMPILERFIXTURE_H
