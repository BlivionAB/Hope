#ifndef ELET_COMPILER_H
#define ELET_COMPILER_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include "Parser.h"
#include "Binder.h"
#include "Syntax.h"
#include "Instruction/Instruction.h"
#include "Instruction/Transformer.h"
#include "Domain/Instruction/AssemblyWriter.h"
#include "Domain/Instruction/ObjectFileWriter.h"
#include <Foundation/Path.h>

namespace elet::domain::compiler::instruction::output
{
    class AssemblyWriter;
}


namespace elet::domain::compiler
{


using namespace instruction;
using namespace instruction::output;


enum class AssemblyTarget
{
    Unknown,
    x86_64,
    Baseline,
};


class Parser;

struct ParsingTask
{
    const char*
    sourceStart;

    const char*
    sourceEnd;

    const Path*
    sourceDirectory;

    File*
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

        File*
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

    std::map<Utf8String, File*>
    files;

private:

    void
    startWorker();

    void
    acceptParsingWork();

    void
    acceptBindingWork();

    void
    acceptTransformationWork();

    void
    acceptAssemblyWritingWork();

    List<Syntax*>
    _syntaxTree;

    std::mutex
    _transformationWorkMutex;

    std::queue<Declaration*>
    _transformationWork;

    std::queue<BindingWork>
    _bindingWork;

    std::queue<output::Routine*>
    _routines;

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

    compiler::Parser*
    _parser;

    Transformer*
    _transformer;

    Binder*
    _binder;

    AssemblyWriter*
    _assemblyWriter;

    std::map<Routine*, List<std::uint8_t>*>
    _output;

    const Path*
    _outputFile;

    unsigned int
    _pendingParsingFiles = 0;

    unsigned int
    _pendingParsingTasks = 0;

    std::map<Utf8StringView, File*>
    _urlToSymbolMap;

    ObjectFileWriter*
    _objectFileWriter;
};


}


#endif //ELET_COMPILER_H
