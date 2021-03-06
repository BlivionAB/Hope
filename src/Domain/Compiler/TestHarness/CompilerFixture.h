#ifndef ELET_COMPILERFIXTURE_H
#define ELET_COMPILERFIXTURE_H


#include <gmock/gmock.h>
#include <fstream>
#include <optional>
#include "MachOBaselineParser.h"
#include <Foundation/File.h>
#include <stdlib.h>
#include <filesystem>
#include "Domain/Compiler/TestHarness/TextDiff/MyersDiff.h"
#include "Domain/Compiler/TestHarness/TextDiff/DiffPrinter.h"
#include "StashIrPrinter.h"
#include "TypeBaselinePrinter.h"


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
        Pe32_x86_64,
        StashIR,
    };


    struct TestProjectOptions
    {

        std::string
        baselineName;

        List<CompilationTarget>
        targets;

        std::optional<bool>
        writeExecutable;

        std::optional<bool>
        printTypeBaseline;

        std::optional<OptimizationLevel>
        optimizationLevel;

        std::optional<bool>
        acceptBaselines;

        std::optional<bool>
        generateTestFunction = true;

        std::optional<StashIrOption>
        stashIrOptions;
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
            return "";
        }


        void SetUp()
        {

        }

        virtual
        std::filesystem::path
        basePath()
        {
            return "src/Domain/Compiler/Tests/";
        }

        TestProject
        project;

        StashIRPrinter
        stashIRPrinter;

        TypeBaselinePrinter
        typeBaselinePrinter;

        std::optional<TestProjectOptions>
        _testOptions;

        Utf8String
        _testSource;

        Utf8String
        _testReturnType;

        void
        testFunction(Utf8String source, Utf8String returnType = "s32")
        {
            _testSource = source;
            _testReturnType = returnType;
        }


        ExpandedCompilerOptions
        getCompilerOptionsFromCompilationTarget(const CompilationTarget compilationTarget, std::optional<StashIrOption> stashIrOption)
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
                case CompilationTarget::Pe32_x86_64:
                    assemblyTarget = AssemblyTarget::x86_64;
                    objectFileTarget = ObjectFileTarget::Pe32;
                    break;
                case CompilationTarget::StashIR:
                    assemblyTarget = AssemblyTarget::StashIR;
                    objectFileTarget = ObjectFileTarget::StashIR;
                    break;
                default:
                    throw std::runtime_error("Unknown compilation target.");
            }
            ExpandedCompilerOptions options(assemblyTarget, objectFileTarget);
            if (compilationTarget == CompilationTarget::StashIR)
            {
                if (stashIrOption)
                {
                    options.assemblyHasMultiRegisterOperands = stashIrOption->assemblyHasMultiRegisterOperands;
                    options.treatModuloAsDivision = stashIrOption->treatModuloAsDivision;
                    options.hasProvidedStashIrOptions = true;
                }
            }
            return options;
        }


        testing::AssertionResult
        testProject(TestProjectOptions options)
        {
            if (*options.generateTestFunction)
            {
                generateTestFunction(options);
            }
            size_t len;
            char* value;
            const char* envvar;
            getenv_s(&len, value, 100, "ACCEPT_BASELINES");
            if (envvar && std::strcmp(envvar, "true") == 0)
            {
                options.acceptBaselines = true;
            }
            testing::AssertionResult result(true);
            for (const auto target : options.targets)
            {
                compileTarget(target, options, result);
            }
            return result;
        }


        void
        generateTestFunction(const TestProjectOptions& options)
        {
            TextWriter tw;
            tw.writeLine("domain Common::MyApplication");
            tw.writeLine("{");
            tw.newline();
            tw.writeLine("public:");
            tw.indent();
            tw.writeLine("fn Test(): " + _testReturnType);
            tw.writeLine("{");
            tw.indent();
            tw.writeIndentedText(_testSource);
            tw.unindent();
            tw.newline();
            tw.write("}");
            if (options.writeExecutable)
            {
                tw.writeLine("fn OnApplicationStart(): void");
                tw.writeLine("{");
                tw.indent();
                tw.write("Test();");
                tw.unindent();
                tw.writeLine("}");
            }
            tw.unindent();
            tw.newline();
            tw.write("}");
            project.setEntrySourceFile("main.hs",tw.toString());
        }


        void
        compileTarget(const CompilationTarget target, const TestProjectOptions& options, testing::AssertionResult& result)
        {
            ExpandedCompilerOptions compilerOptions = getCompilerOptionsFromCompilationTarget(target, options.stashIrOptions);
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
                if (options.printTypeBaseline)
                {
                    List<ast::SourceFile*> sourceFiles = compiler.getSourceFiles();
                    Utf8String typeBaseline = typeBaselinePrinter.printSourceFiles(sourceFiles);
                    checkBaseline(typeBaseline, options, compilerOptions, result, "types");
                }
                else if (compilerOptions.assemblyTarget == AssemblyTarget::StashIR)
                {
                    std::queue<output::FunctionRoutine*> output = compiler.getStashIR();
                    checkStashIRBaseline(output, options, compilerOptions, result);
                }
                else
                {
                    if (options.writeExecutable)
                    {
                        fs::path objectFilePath(options.baselineName + ".obj");
                        fs::path executableFilePath(options.baselineName + ".exe");
                        compiler.writeObjectFile(objectFilePath);
                        compiler.link(objectFilePath, executableFilePath);
                    }
                    else
                    {
                        List<uint8_t>& output = compiler.getOutput();
                        switch (compilerOptions.objectFileTarget)
                        {
                            case ObjectFileTarget::MachO:
                                switch (compilerOptions.assemblyTarget)
                                {
                                    case AssemblyTarget::x86_64:
                                        checkTextSegmentBaselines<x86::X86AssemblyParser, x86::X86AssemblyPrinter, x86::Instruction>(options, compilerOptions, output, result);
                                        break;
                                    case AssemblyTarget::Aarch64:
                                        checkTextSegmentBaselines<Aarch64Parser, Aarch64Printer, OneOfInstruction>(options, compilerOptions, output,result);
                                        break;
                                    default:
                                        throw std::runtime_error("Unknown assembly target for MachO.");
                                }
                                break;
                            case ObjectFileTarget::Pe32:
                                throw std::runtime_error("We have not implemented Pe32 baseline tests yet.");
                            default:
                                throw std::runtime_error("We have not implemented Pe32 baseline tests yet.");
                        }
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
                        default:
                            throw std::runtime_error("Architecture not supported.");
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Unknown target in getArchitecture.");
            }
        }


        template<typename TAssemblyParser, typename TAssemblyPrinter, typename TInstruction>
        void
        checkTextSegmentBaselines(const TestProjectOptions& options, const CompilerOptions& compilerOptions, List<uint8_t>& output, testing::AssertionResult& result)
        {
            MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TInstruction> baselineParser(output, compilerOptions.assemblyTarget);
            baselineParser.parse();
            Utf8String textSection = baselineParser.serializeTextSegment();
            checkBaseline(textSection, options, compilerOptions, result, "basm");
        }


        void
        checkBaseline(Utf8String& result, const TestProjectOptions& testOptions, const CompilerOptions& compilerOptions, testing::AssertionResult& testResult, std::string suffix)
        {
            DiffPrinter printer;
            MyersDiff differ;
            fs::path basm = fs::current_path() / ".." / basePath() / localTestPath() / "__Baselines__" / (testOptions.baselineName + "-" + getArchitecture(compilerOptions) + "." + suffix);
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
                if (testOptions.acceptBaselines)
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
        checkStashIRBaseline(std::queue<output::FunctionRoutine*>& output, const TestProjectOptions& options, const CompilerOptions& compilerOptions, testing::AssertionResult& result)
        {
            Utf8String baselineOutput = stashIRPrinter.writeFunctionRoutines(output);
            checkBaseline(baselineOutput, options, compilerOptions, result, "basm");
        }
    };
}

#endif //ELET_COMPILERFIXTURE_H
