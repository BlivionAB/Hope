#include "Compiler.h"
#include "Parser.h"
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <sys/stat.h>
#include <Foundation/Path.h>
#include <Foundation/File.h>

Compiler::Compiler():
    _parser(new Parser(this))
{ }

void
Compiler::compile(const char* file)
{
    parseFile(file);
};

void
Compiler::parseStandardLibrary()
{
    _compilationStage = CompilationStage::Parsing;
    Path iostreamPath = Path::relative(Path::cwd(), "src/Domain/StandardLibrary/Module.l1");
    parseFile(iostreamPath);
}

void
Compiler::parseFile(const Path& file)
{
    static const std::size_t BUFFER_SIZE = 16*1024;
    int fd = open(file.toString().asString(), O_RDONLY);
    if (fd == -1)
    {
        throw FileReadError();
    }

#ifndef __APPLE__
    posix_fadvise(fd, 0, 0, FDADVICE_SEQUENTIAL);
#endif

    struct stat sb;
    fstat(fd, &sb);
    _source = new char[sb.st_size];
    _readCursor = const_cast<char*>(_source);
    Path cd = Path::directory(file);
    Path* currentDirectory = new Path(cd);
    while (size_t bytesRead = ::read(fd, _readCursor, BUFFER_SIZE))
    {
        if (bytesRead < BUFFER_SIZE)
        {
            _reachedEndOfFile = true;
        }
        char* endCursor = &_readCursor[bytesRead];
        _parseTasks.emplace(_readCursor, endCursor, currentDirectory);
        _condition.notify_one();
        _readCursor = endCursor;
        if (_reachedEndOfFile)
        {
            break;
        }
    }
}

void
Compiler::compilationWorker()
{
    {
        std::lock_guard<std::mutex> lock(_threadWaitingMutex);
        _numberOfThreadsWaiting++;
    }
    _threadsWaitingCondition.notify_one();
    if (_compilationStage == CompilationStage::Parsing)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mutex);
            _condition.wait(lock, [&]
            {
                return !_parseTasks.empty() || _closed;
            });
            if (_parseTasks.empty() && _closed)
            {
                return;
            }
            ParseTask result = _parseTasks.front();
            List<Syntax*> statements = _parser->parse(result.start, result.end, result.directory);
            for (Syntax* statement : statements)
            {
                _syntaxTree.add(statement);
            }
            _parseTasks.pop();
        }
    }
}

void
Compiler::startWorkers()
{
    _compilationStage = CompilationStage::Parsing;
    int numberOfThreads = 2;
    for (int i = 0; i < numberOfThreads; i++)
    {
        _workers.emplace_back(&Compiler::compilationWorker, this);
    }
    {
        std::unique_lock<std::mutex> lock(_threadWaitingMutex);
        _threadsWaitingCondition.wait(lock, [&]
        {
            return _numberOfThreadsWaiting == numberOfThreads;
        });
    }
}

void
Compiler::endWorkers()
{
    _closed = true;
    _condition.notify_all();
    for (auto& worker : _workers)
    {
        worker.join();
    }
}
