#include "Compiler.h"
#include "Parser.h"
#include "Binder.h"
#include "Checker.h"
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <sys/stat.h>
#include <Foundation/FilePath.h>
#include <Foundation/File.h>
#include "Instruction/ObjectFileWriter/BaselineObjectFileWriter.h"

using namespace elet::domain::compiler::instruction::output;

namespace elet::domain::compiler
{


Compiler::Compiler(FileReader& fileReader, AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget):
    _fileReader(fileReader),
    _assemblyTarget(assemblyTarget),
    _objectFileTarget(objectFileTarget),
    _parser(new ast::Parser(this)),
    _assemblyWriter(new AssemblyWriter(assemblyTarget)),
    _binder(new Binder()),
    _checker(new Checker(_binder))
{
    _transformer = new Transformer(_dataMutex);
    _objectFileWriter = new BaselineObjectFileWriter();
}

void
Compiler::compileFile(const FilePath& file, const FilePath& output)
{
    _outputFile = &output;
    addFile(file);
}


void
Compiler::addFile(const FilePath& file)
{
    Utf8String fileString = file.toString();
    auto result = files.find(fileString);
    if (result != files.end())
    {
        return;
    }
    ast::SourceFile* compilerFile = new ast::SourceFile();
    files.insert(std::pair(fileString, compilerFile));
    _pendingParsingFiles++;
    static const std::size_t BUFFER_SIZE = 16*1024;
    int fd = _fileReader.openFile(file);
    if (fd == -1)
    {
        throw FileReadError();
    }

#ifndef __APPLE__
    posix_fadvise(fd, 0, 0, FDADVICE_SEQUENTIAL);
#endif

    size_t bufferSize = _fileReader.getFileSize(fd);
    _source = new char[bufferSize];
    _readCursor = const_cast<char*>(_source);
    FilePath cd(file);
    FilePath* currentDirectory = new FilePath(cd);
    while (size_t bytesRead = _fileReader.readChunk(fd, _readCursor, BUFFER_SIZE))
    {
        bool isEndOfFile = false;
        if (bytesRead < BUFFER_SIZE)
        {
            isEndOfFile = _reachedEndOfFile = true;
        }
        char* endCursor = &_readCursor[bytesRead];
        _parsingWork.emplace(_readCursor, endCursor, currentDirectory, compilerFile, isEndOfFile);
        _parsingWorkCondition.notify_one();
        _readCursor = endCursor;
        if (_reachedEndOfFile)
        {
            break;
        }
    }
}


void
Compiler::startWorker()
{
    {
        std::lock_guard<std::mutex> lock(_threadWaitingMutex);
        _numberOfWorkersWaiting++;
    }
    _startWorkerCondition.notify_one();
    acceptParsingWork();
    acceptBindingWork();
    acceptCheckingWork();
    acceptTransformationWork();
    acceptAssemblyWritingWork();
    _display_mutex.lock();
//    std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//    std::cout << "cleanup phase"<< std::endl;
    _display_mutex.unlock();
    _finishedWork = true;

    // We have to notify all conditions due to some awaiting while one thread can finish all work.
    // Note also, parsing work is the only stage where the thread is awaiting for work from the file
    // read.
    _parsingWorkCondition.notify_all();

    // Notify main thread that the work is done. The main thread is awaiting for all work to be done
    // before joining all children threads.
    _finishWorkCondition.notify_one();
}


void
Compiler::startWorkers()
{
    _compilationStage = CompilationStage::Parsing;
    int numberOfWorkers = 1;
    for (int i = 0; i < numberOfWorkers; i++)
    {
        _workers.emplace_back(&Compiler::startWorker, this);
    }
    {
        std::unique_lock<std::mutex> lock(_threadWaitingMutex);
        _startWorkerCondition.wait(lock, [&]
        {
            return _numberOfWorkersWaiting == numberOfWorkers;
        });
    }
}

void
Compiler::endWorkers()
{
    std::unique_lock finishWorkLock(_finishWorkMutex);
    _finishWorkCondition.wait(finishWorkLock, [&]
    {
//        _display_mutex.lock();
//        std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//        std::cout << "finished work: " << _finishedWork << std::endl;
//        _display_mutex.unlock();
        return _finishedWork;
    });
    for (auto& worker : _workers)
    {
        worker.join();
    }
//    for (auto some : _text)
//    {
//        std::cout << *(std::uint8_t*)some.second << std::endl;
//    }
//    std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//    std::cout << "join workers" << std::endl;

}


void
Compiler::acceptParsingWork()
{
    _parser->symbols = new List<Symbol*>();
    while (_compilationStage == CompilationStage::Parsing)
    {
        std::unique_lock<std::mutex> lock(_parsingWorkMutex);
        _parsingWorkCondition.wait(lock, [&]
        {
            return !_parsingWork.empty() || !_pendingParsingWork.empty() || (_pendingParsingFiles == 0 && _pendingParsingTasks == 0);
        });
        if (_compilationStage != CompilationStage::Parsing)
        {
            break;
        }
        if (!_parsingWork.empty())
        {
            _pendingParsingTasks++;
            ParsingTask task = _parsingWork.front();
            _parsingWork.pop();
            lock.unlock();
            auto result = _parser->performWork(task);
            if (result.pendingParsingTask)
            {
                if (!task.isEndOfFile)
                {
                    _pendingParsingWork.emplace(task.sourceEnd, result.pendingParsingTask);
                }
            }
            else
            {
                if (task.isEndOfFile)
                {
                    _pendingParsingFiles--;
                }
            }
            pushBindingWork(result.statements);
            _pendingParsingTasks--;
            // Break early, instead of waiting for condition, that might not fire.
            if (_pendingParsingFiles == 0 && _pendingParsingTasks == 0)
            {
                _compilationStage = CompilationStage::Binding;
                _parsingWorkCondition.notify_all();
                break;
            }
        }
    }

    {
        std::unique_lock symbolOffsetWorkLock(_symbolOffsetWorkMutex);
        std::uint64_t thisThreadTotalOffset = 1;
        for (Symbol* symbol : *_parser->symbols)
        {
            thisThreadTotalOffset += symbol->identifier.size() + 1;
            symbol->textOffset += _symbolSectionOffset;
            symbol->index = _symbolIndex;
            _symbols.add(symbol);
        }
        _symbolSectionOffset += thisThreadTotalOffset;
    }
}

void
Compiler::pushBindingWork(const List<ast::Syntax*> statements)
{
    for (ast::Syntax* statement : statements)
    {
        if (statement->labels & LABEL__DECLARATION)
        {
            auto declaration = reinterpret_cast<ast::Declaration*>(statement);
            if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
            {
                auto functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(declaration);
                _bindingWork.push(new BindingWork(functionDeclaration));
            }
            else if (declaration->kind == ast::SyntaxKind::DomainDeclaration)
            {
                auto domain = reinterpret_cast<ast::DomainDeclaration*>(declaration);
                _bindingWork.push(new BindingWork(domain));
            }
        }
        else if (statement->labels & LABEL__USING_STATEMENT)
        {
            _bindingWork.push(new BindingWork(reinterpret_cast<ast::UsingStatement*>(statement)));
        }
        _syntaxTree.add(statement);
    }
}


void
Compiler::acceptBindingWork()
{
    while (_compilationStage == CompilationStage::Binding)
    {
        _bindingWorkMutex.lock();
        if (_bindingWork.empty())
        {
            _bindingWorkMutex.unlock();
            if (_pendingBindingWork == 0)
            {
                _compilationStage = CompilationStage::Checking;
                break;
            }
            continue;
        }
        BindingWork* work = _bindingWork.front();
        _bindingWork.pop();
        _bindingWorkMutex.unlock();
        _pendingBindingWork++;
        _binder->performWork(*work, &_parser->domainDeclarationMap);
        _checkingWork.push(work->declaration);
        _pendingBindingWork--;
    }
}


void
Compiler::acceptCheckingWork()
{
    while (_compilationStage == CompilationStage::Checking)
    {
        _checkingWorkMutex.lock();
        if (_checkingWork.empty())
        {
            _checkingWorkMutex.unlock();
            if (_pendingBindingWork == 0)
            {
                _compilationStage = CompilationStage::Transformation;
                break;
            }
            continue;
        }
        ast::Syntax* statement = _checkingWork.front();
        _checkingWork.pop();
        _checkingWorkMutex.unlock();
        if (statement->labels & LABEL__DECLARATION)
        {
            auto declaration = reinterpret_cast<ast::Declaration*>(statement);
            _checker->checkTopLevelDeclaration(declaration);
            if (declaration->kind == ast::SyntaxKind::DomainDeclaration)
            {
                auto domain = reinterpret_cast<ast::DomainDeclaration*>(declaration);
                for (const auto& decl : domain->block->declarations)
                {
                    if (decl->kind == ast::SyntaxKind::FunctionDeclaration)
                    {
                        _transformationWork.push(reinterpret_cast<ast::FunctionDeclaration*>(decl));
                    }
                }
            }
            else if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
            {
                _transformationWork.push(reinterpret_cast<ast::FunctionDeclaration*>(declaration));
            }
        }
        else if (statement->labels & LABEL__USING_STATEMENT)
        {
            _checker->checkUsingStatement(reinterpret_cast<ast::UsingStatement*>(statement));
        }
    }
}


void
Compiler::acceptTransformationWork()
{
    while (_compilationStage == CompilationStage::Transformation)
    {
//        _display_mutex.lock();
//        std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//        std::cout << "locked down the thread"<< std::endl;
//        _display_mutex.unlock();
        _transformationWorkMutex.lock();
        if (_transformationWork.empty())
        {
//            _display_mutex.lock();
//            std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//            std::cout << "unlocked by empty"<< std::endl;
//            _display_mutex.unlock();
            _transformationWorkMutex.unlock();
            if (_pendingTransformationTasks == 0)
            {
//                _display_mutex.lock();
//                std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//                std::cout << "begin writing work"<< std::endl;
//                _display_mutex.unlock();
                _compilationStage = CompilationStage::Writing;
                break;
            }
            continue;
        }
        ast::Declaration* declaration = _transformationWork.front();
        _transformationWork.pop();
//        _display_mutex.lock();
//        std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//        std::cout << "unlocked by work"<< std::endl;
//        _display_mutex.unlock();
        _transformationWorkMutex.unlock();
//        _display_mutex.lock();
//        std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//        std::cout << "taking transformation work"<< std::endl;
//        _display_mutex.unlock();
        _pendingTransformationTasks++;
//        _display_mutex.lock();
//        std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//        std::cout << "Transform declaration->kind " << (int)declaration->kind << std::endl;
//        _display_mutex.unlock();
        output::FunctionRoutine* routine = _transformer->transform(declaration);
        if (routine->isStartFunction)
        {
            _routines.push(routine);
        }
        _pendingTransformationTasks--;
    }
}


void
Compiler::acceptAssemblyWritingWork()
{
    while (_compilationStage == CompilationStage::Writing)
    {
        if (_routines.empty())
        {
            break;
        }
        output::FunctionRoutine* routine = _routines.front();
        _objectFileWriter->write(routine);
        _routines.pop();
    }
}

List<uint8_t>&
Compiler::getOutput()
{
    return _objectFileWriter->getOutput();
}


}