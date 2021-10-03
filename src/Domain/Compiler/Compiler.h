#ifndef ELET_COMPILER_H
#define ELET_COMPILER_H

#include <map>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>
#include <queue>
#include "Parser.h"
#include "Binder.h"
#include "Checker.h"
#include "Syntax/Syntax.h"
#include "Domain/Compiler/Instruction/Instruction.h"
#include "Domain/Compiler/Instruction/Transformer.h"
#include "Domain/Compiler/Instruction/AssemblyWriter.h"
#include "Domain/Compiler/Instruction/ObjectFileWriter.h"
#include "CompilerTypes.h"
#include <Foundation/FileReader.h>


namespace fs = std::filesystem;


namespace elet::domain::compiler::instruction::output
{
    class AssemblyWriter;
    class ObjectFileWriterInterface;
    struct RelocationOperand;
    struct FunctionRoutine;
}


namespace elet::domain::compiler::instruction
{
    class Transformer;
}

namespace elet::domain::compiler
{
    namespace ast
    {
        class Binder;
        class Checker;
        class Parser;
        struct BindingWork;
    }


    using namespace elet::domain::compiler::instruction;


    enum class AssemblyTarget
    {
        Unknown,
        x86_64,
        Aarch64,
        StashIR,
    };


    struct ParsingTask
    {
        const char*
        sourceStart;

        const char*
        sourceEnd;

        const fs::path*
        sourceDirectory;

        ast::SourceFile*
        sourceFile;

        bool
        isEndOfFile;

        ParsingTask(
            const char*
            start,

            const char*
            end,

            const fs::path*
            directory,

            ast::SourceFile*
            file,

            bool
            endOfFile
        ):
            sourceStart(start),
            sourceEnd(end),
            sourceDirectory(directory),
            sourceFile(file),
            isEndOfFile(endOfFile)
        { }
    };


    struct PendingParsingTask
    {
        const char*
        pendingBlock;

        ParsingTask*
        task;

        PendingParsingTask(const char* pendingBlock, ParsingTask* task):
            pendingBlock(pendingBlock),
            task(task)
        { }
    };


    struct RelativeRelocationTask
    {
        output::FunctionRoutine*
        routine;

        std::uint64_t
        offset;

        output::RelocationOperand*
        relocation;

        RelativeRelocationTask(output::FunctionRoutine* routine, std::uint64_t offset, output::RelocationOperand* relocation):
            routine(routine),
            offset(offset),
            relocation(relocation)
        { }
    };


    enum class SymbolSectionIndex : std::uint8_t
    {
        Text,
        Const,
    };


    struct Symbol
    {
        std::uint32_t
        index = 0;

        const
        Utf8StringView
        identifier;

        Utf8StringView
        name;

        Utf8StringView
        externalSymbol;

        std::uint32_t
        textOffset;

        Symbol(std::uint32_t textOffset, const Utf8StringView name):
            textOffset(textOffset),
            name(name)
        { }
    };


    struct CompilerOptions
    {
        AssemblyTarget
        assemblyTarget;

        ObjectFileTarget
        objectFileTarget;

        CompilerOptions(AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget):
            assemblyTarget(assemblyTarget),
            objectFileTarget(objectFileTarget)
        { }
    };


    // Note: there are no checking in CLion, due to forward declaration of class Compiler in Parser.h
    class Compiler
    {
    public:

        Compiler(
            FileStreamReader& fileReader,
            CompilerOptions options);

        void
        startWorkers();

        void
        endWorkers();

        void
        compileFile(const fs::path& file);

        std::map<std::string, ast::SourceFile*>
        files;

        std::queue<output::FunctionRoutine*>
        getStashIR();

        List<uint8_t>&
        getOutput();

    private:

        void
        startWorker();

        void
        acceptParsingWork();

        void
        acceptBindingWork();

        void
        acceptCheckingWork();

        void
        acceptTransformationWork();

        void
        acceptAssemblyWritingWork();

        void
        pushBindingWork(const List<ast::Syntax*> statements);

        List<Diagnostic>
        _diagnostics;

        CompilerOptions
        _options;

        List<ast::Syntax*>
        _syntaxTree;

        FileStreamReader&
        _fileStreamReader;

        std::mutex
        _transformationWorkMutex;

        std::queue<ast::Syntax*>
        _checkingWork;

        std::queue<ast::FunctionDeclaration*>
        _transformationWork;

        std::queue<ast::BindingWork*>
        _bindingWork;

        std::queue<output::FunctionRoutine*>
        _routines;

        List<Utf8StringView*>
        _data;

        std::uint64_t
        _cstringOffset = 0;

        std::mutex
        _dataMutex;

        std::vector<std::thread>
        _workers;

        AssemblyTarget
        _assemblyTarget;

        ObjectFileTarget
        _objectFileTarget;

        bool
        _reachedEndOfFile = false;

        std::condition_variable
        _parsingWorkCondition;

        std::mutex
        _parsingWorkMutex;

        enum class CompilationStage
        {
            Parsing,
            Binding,
            Checking,
            Transformation,
            Writing,
        }
        _compilationStage;

        std::mutex
        _writingWorkMutex;

        std::mutex
        _bindingWorkMutex;

        std::mutex
        _checkingWorkMutex;

        std::mutex
        _display_mutex;

        unsigned int
        _pendingWritingTasks = 0;

        unsigned int
        _pendingTransformationTasks = 0;

        unsigned int
        _pendingBindingWork = 0;

        const char*
        _source;

        std::queue<ParsingTask>
        _parsingWork;

        std::queue<PendingParsingTask>
        _pendingParsingWork;

        std::mutex
        _threadWaitingMutex;

        std::condition_variable
        _startWorkerCondition;

        std::mutex
        _finishWorkMutex;

        std::condition_variable
        _finishWorkCondition;

        bool
        _finishedWork = false;

        int
        _numberOfWorkersWaiting = 0;

        char*
        _readCursor;

        bool
        _modifed = false;

        ast::Parser*
        _parser;

        instruction::Transformer*
        _transformer;

        ast::Binder*
        _binder;

        ast::Checker*
        _checker;

        List<output::FunctionRoutine*>
        _output;

        std::mutex
        _outputAdditionMutex;

        std::uint64_t
        _outputSize = 0;

        List<output::RelocationOperand*>
        _relativeRelocations;

        List<output::RelocationOperand*>
        _symbolicRelocations;

        const fs::path*
        _outputFile;

        unsigned int
        _pendingParsingFiles = 0;

        unsigned int
        _pendingParsingTasks = 0;

        std::map<Utf8StringView, ast::SourceFile*>
        _urlToSymbolMap;

        output::ObjectFileWriterInterface*
        _objectFileWriter;

        List<Utf8StringView*>
        _cstrings;

        List<Symbol*>
        _symbols;

        std::uint32_t
        _symbolIndex = 0;

        std::multimap<Utf8StringView, Symbol*>
        _symbolMap;

        std::uint64_t
        _symbolSectionOffset = 0;

        std::mutex
        _symbolOffsetWorkMutex;

        std::uint64_t
        _relocationOffset = 0;

        std::mutex
        _relocationWorkMutex;
    };
}


#endif //ELET_COMPILER_H
