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
#include "Domain/Compiler/Instruction/Optimizer.h"
#include "Domain/Compiler/Instruction/AssemblyWriter.h"
#include "CompilerTypes.h"
#include "ErrorWriter.h"
#include "Domain/Compiler/Error/Error.h"
#include <Foundation/FileReader.h>


namespace fs = std::filesystem;


namespace elet::domain::compiler::instruction::output
{
    class AssemblyWriter;
    class ObjectFileWriter;
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
        class ErrorWriter;
        struct BindingWork;
    }


    using namespace elet::domain::compiler::instruction;


    enum class SymbolSectionIndex : std::uint8_t
    {
        Text,
        Const,
    };


    // Note: there are no checking in CLion, due to forward declaration of class Compiler in Parser.h
    class Compiler
    {
    public:

        Compiler(
            FileStreamReader& fileReader,
            CompilerOptions& options);

        void
        startWorkers();

        void
        endWorkers();

        void
        compileFile(const fs::path& file);

        void
        writeObjectFile(fs::path filePath) const;

        std::map<std::string, ast::SourceFile*>
        files;

        std::queue<output::FunctionRoutine*>
        getStashIR();

        List<uint8_t>&
        getOutput() const;

        Utf8String
        printCompileErrors();

        List<ast::SourceFile*>
        getSourceFiles();

        void
        link(const fs::path& objectFilePath, const fs::path& executableFilePath) const;

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
        acceptOptimizationWork();

        void
        acceptAssemblyWritingWork();

        void
        pushBindingWork(const List<ast::Syntax*> statements);

        ast::ErrorWriter
        _errorPrinter;

        CompilerOptions
        _options;

        List<ast::Syntax*>
        _syntaxTree;

        FileStreamReader&
        _fileStreamReader;

        std::mutex
        _transformationWorkMutex;

        std::mutex
        _optimizationWorkMutex;

        std::queue<ast::Syntax*>
        _checkingWork;

        std::queue<ast::FunctionDeclaration*>
        _transformationWork;

        std::queue<ast::BindingWork*>
        _bindingWork;

        std::queue<output::FunctionRoutine*>
        _optimizationWork;

        std::queue<output::FunctionRoutine*>
        _routines;

        List<ast::SourceFile*>
        _sourceFiles;

        List<Utf8StringView*>
        _data;

        std::uint64_t
        _cstringOffset = 0;

        std::mutex
        _dataMutex;

        std::vector<std::thread>
        _workers;

        List<ast::error::SyntaxError*>
        _syntaxErrors;

        List<ast::error::LexicalError*>
        _lexicalErrors;

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
            Optimization,
            Writing,

            Error,
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
        _pendingOptimizationTasks = 0;

        unsigned int
        _pendingBindingWork = 0;

        const char*
        _source;

        std::queue<ast::SourceFile*>
        _parsingWork;

        std::queue<ast::PendingParsingTask>
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

        instruction::output::Optimizer*
        _optimizer;

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
        _pendingParsingTasks = 0;

        std::map<Utf8StringView, ast::SourceFile*>
        _urlToSymbolMap;

        output::ObjectFileWriter*
        _objectFileWriter;

        List<Utf8StringView*>
        _cstrings;

        List<ast::Symbol*>
        _symbols;

        std::uint32_t
        _symbolIndex = 0;

        std::multimap<Utf8StringView, ast::Symbol*>
        _symbolMap;

        std::uint64_t
        _symbolSectionOffset = 0;

        std::mutex
        _symbolOffsetWorkMutex;

        std::uint64_t
        _relocationOffset = 0;

        std::mutex
        _relocationWorkMutex;

        const output::Optimizer::Options
        getOptimizerOptions(CompilerOptions options);

        output::ObjectFileWriter*
        createObjectFileWriter(CompilerOptions options);
    };
}



#endif //ELET_COMPILER_H
