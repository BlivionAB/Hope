#ifndef ELET_COMPILER_H
#define ELET_COMPILER_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include "Parser.h"
#include <Foundation/Path.h>
#include "Mir.h"

class Parser;

struct ParseTask
{
    const char*
    start;

    const char*
    end;

    const Path*
    directory;

    ParseTask(
        const char*
        start,

        const char*
        end,

        const Path*
        directory
    ):
        start(start),
        end(end),
        directory(directory)
    { }
};

class Compiler
{
public:

    Compiler();

    void
    compile(const char* file);

    void
    startWorkers();

    void
    endWorkers();

    void
    parseFile(const Path& file);

//    void
//    addFile(const Path& file);

    void
    parseStandardLibrary();

private:

    void
    compilationWorker();

    List<Syntax*>
    _syntaxTree;

    std::vector<std::thread>
    _workers;

    bool
    _reachedEndOfFile = false;

    std::condition_variable
    _condition;

    std::mutex
    mutex;

    enum class CompilationStage {
        Parsing,
        Checking,
    }
    _compilationStage;

    const char*
    _source;

    std::queue<ParseTask>
    _parseTasks;

    std::mutex
    _threadWaitingMutex;

    std::condition_variable
    _threadsWaitingCondition;

    int
    _numberOfThreadsWaiting = 0;

    char*
    _readCursor;

    bool
    _modifed = false;

    Parser*
    _parser;

    bool
    _closed = false;
};


#endif //ELET_COMPILER_H
