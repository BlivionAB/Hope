#ifndef ELET_COMPILER_H
#define ELET_COMPILER_H

#include <map>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include "Parser.h"
#include "Binder.h"
#include "Syntax.h"
#include "Checker.h"
#include "Instruction/Instruction.h"
#include "Instruction/Transformer.h"
#include "Domain/Instruction/AssemblyWriter.h"
#include "Domain/Instruction/ObjectFileWriter.h"
#include <Foundation/Path.h>

namespace elet::domain::compiler::instruction::output
{
    class AssemblyWriter;
    class ObjectFileWriter;
    struct StringReference;
    struct RelocationOperand;
    struct FunctionReference;
}
namespace elet::domain::compiler::instruction
{
    class Transformer;

    namespace output
    {
        struct Routine;
        enum class ObjectFileTarget;
    }
}

namespace elet::domain::compiler
{


class Binder;
class Checker;
struct DeclarationWork;


namespace ast
{
    class Parser;
}


using namespace instruction;
using namespace instruction::output;


enum class AssemblyTarget
{
    Unknown,
    x86_64,
    Baseline,
};

struct ParsingTask
{
    const char*
    sourceStart;

    const char*
    sourceEnd;

    const Path*
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

        const Path*
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
    output::Routine*
    routine;

    std::uint64_t
    offset;

    RelocationOperand*
    relocation;

    RelativeRelocationTask(output::Routine* routine, std::uint64_t offset, RelocationOperand* relocation):
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

    SymbolSectionIndex
    sectionIndex;

    std::uint32_t
    textOffset;

    Symbol(SymbolSectionIndex sectionIndex, std::uint32_t textOffset, const Utf8StringView name):
        sectionIndex(sectionIndex),
        textOffset(textOffset),
        name(name)
    { }
};


// Note: there are no checking in CLion, due to forward declaration of class Compiler in Parser.h
class Compiler
{
public:

    Compiler(AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget);

    void
    startWorkers();

    void
    endWorkers();

    void
    compileFile(const Path& file, const Path& output);

    void
    addFile(const Path& file);

    void
    writeToOutputFile();

    void
    parseStandardLibrary();

    std::map<Utf8String, ast::SourceFile*>
    files;

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
    relocateRelatively(RelocationOperand* relocation, output::Routine* routine, List<RelativeRelocationTask>& relativeRelocationTasks);

    void
    relocateSymbolically(FunctionReference* relocation);

    List<ast::Syntax*>
    _syntaxTree;

    std::mutex
    _transformationWorkMutex;

    std::queue<ast::Declaration*>
    _checkingWork;

    std::queue<ast::Declaration*>
    _transformationWork;

    std::queue<DeclarationWork*>
    _bindingWork;

    std::queue<output::Routine*>
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

    enum class CompilationStage {
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

    Transformer*
    _transformer;

    Binder*
    _binder;

    Checker*
    _checker;

    AssemblyWriter*
    _assemblyWriter;

    List<output::Routine*>
    _output;

    std::mutex
    _outputAdditionMutex;

    std::uint64_t
    _outputSize = 0;

    List<RelocationOperand*>
    _relativeRelocations;

    List<RelocationOperand*>
    _symbolicRelocations;

    const Path*
    _outputFile;

    unsigned int
    _pendingParsingFiles = 0;

    unsigned int
    _pendingParsingTasks = 0;

    std::map<Utf8StringView, ast::SourceFile*>
    _urlToSymbolMap;

    ObjectFileWriter*
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
