#include "Compiler.h"
#include "Parser.h"
#include "Binder.h"
#include "Checker.h"
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <sys/stat.h>
#include <Foundation/Path.h>
#include <Foundation/File.h>


namespace elet::domain::compiler
{


Compiler::Compiler(AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget):
    _assemblyTarget(assemblyTarget),
    _objectFileTarget(objectFileTarget),
    _parser(new ast::Parser(this)),
    _assemblyWriter(new AssemblyWriter(assemblyTarget)),
    _objectFileWriter(new ObjectFileWriter(objectFileTarget)),
    _binder(new Binder()),
    _checker(new Checker())
{
    const auto callingConvention = new CallingConvention {
       { 7, 6, 3, 2, 8, 9 },
       6,
       { 0 },
       1,
    };
    _transformer = new Transformer(callingConvention, _routines, _transformationWorkMutex, &_cstrings, _cstringOffset, _dataMutex);
}


void
Compiler::parseStandardLibrary()
{
    _compilationStage = CompilationStage::Parsing;
    Path iostreamPath = Path::resolve(Path::cwd(), "src/Domain/StandardLibrary/Module.l1");
//    compileFile(iostreamPath);
}


void
Compiler::compileFile(const Path& file, const Path& output)
{
    _outputFile = &output;
    addFile(file);
}


void
Compiler::addFile(const Path& file)
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
    Path cd = Path::folderOf(file);
    Path* currentDirectory = new Path(cd);
    while (size_t bytesRead = ::read(fd, _readCursor, BUFFER_SIZE))
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
    writeToOutputFile();
//    for (auto some : _output)
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
            pushBindingWork(result.statements, task.sourceFile);
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
Compiler::pushBindingWork(const List<ast::Syntax*> statements, ast::SourceFile* sourceFile)
{
    for (ast::Syntax* statement : statements)
    {
        if (statement->labels & DECLARATION_LABEL)
        {
            auto declaration = reinterpret_cast<ast::Declaration*>(statement);
            if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
            {
                auto functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(declaration);
                _bindingWork.push(new DeclarationWork(functionDeclaration, sourceFile));
            }
            else if (declaration->kind == ast::SyntaxKind::DomainDeclaration)
            {
                auto domain = reinterpret_cast<ast::DomainDeclaration*>(declaration);
                pushBindingWork(domain->block->declarations, sourceFile);
            }
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
        DeclarationWork* work = _bindingWork.front();
        _bindingWork.pop();
        _bindingWorkMutex.unlock();
        _pendingBindingWork++;
        _binder->performWork(*work);
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
        ast::Declaration* declaration = _checkingWork.front();
        _checkingWork.pop();
        _checkingWorkMutex.unlock();
        _checker->checkTopLevelDeclaration(declaration);
        _transformationWork.push(declaration);
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
        _routines.push(_transformer->transform(declaration));
        _pendingTransformationTasks--;
    }
}


void
Compiler::acceptAssemblyWritingWork()
{
//    _display_mutex.lock();
//    std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//    std::cout << "entered writing work"<< std::endl;
//    _display_mutex.unlock();
    while (_compilationStage == CompilationStage::Writing)
    {
        _writingWorkMutex.lock();
        if (_routines.empty())
        {
            _writingWorkMutex.unlock();
            if (_pendingWritingTasks == 0)
            {
//                _display_mutex.lock();
//                std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//                std::cout << "finished writing work"<< std::endl;
//                _display_mutex.unlock();
                break;
            }
            continue;
        }
        output::Routine* routine = _routines.front();
        _routines.pop();
        _writingWorkMutex.unlock();
//        _display_mutex.lock();
//        std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
//        std::cout << "taking writing work"<< std::endl;
//        _display_mutex.unlock();
        _pendingWritingTasks++;
        _assemblyWriter->writeRoutine(routine);
        _outputAdditionMutex.lock();
        _output.add(routine);
        _outputAdditionMutex.unlock();
        _pendingWritingTasks--;
    }
}

void
Compiler::writeToOutputFile()
{
    List<RelativeRelocationTask> relativeRelocationTasks;
    for (const auto routine : _output)
    {
        routine->symbol->textOffset = _outputSize;
        for (const auto relocation : *routine->symbolicRelocations)
        {
            relocateSymbolically(reinterpret_cast<FunctionReference*>(relocation));
        }
        for (const auto relocation : *routine->relativeRelocations)
        {
            relocateRelatively(relocation, routine, relativeRelocationTasks);
        }
        auto size = routine->machineInstructions->size();
        _outputSize += size;
    }

    // Relocate relatively
    for (const RelativeRelocationTask& relocationTask : relativeRelocationTasks)
    {
        if (relocationTask.relocation->kind == OperandKind::StringReference)
        {
            auto address = reinterpret_cast<std::uint32_t*>(&(*relocationTask.routine->machineInstructions)[relocationTask.offset]);
            *address = (std::uint32_t)(_outputSize - relocationTask.relocation->textOffset + 4) + relocationTask.relocation->dataOffset;
        }
    }
    auto directory = Path::folderOf(*_outputFile);
    if (!Path::exists(directory))
    {
        foundation::File::createDirectory(directory);
    }
    const AssemblySegments segments = {
        &_output,
        _outputSize,
        &_cstrings,
        _cstringOffset,
        &_symbols,
        _symbolSectionOffset,
        &_symbolicRelocations,
        &_relativeRelocations,
    };
    _objectFileWriter->writeToFile(*_outputFile, segments);
}


inline
void
Compiler::relocateSymbolically(FunctionReference* relocation)
{
    relocation->textOffset += _outputSize;
    _symbolicRelocations.add(relocation);
}


inline
void
Compiler::relocateRelatively(RelocationOperand* relocation, output::Routine* routine, List<RelativeRelocationTask>& relativeRelocationTasks)
{
    relativeRelocationTasks.emplace(routine, relocation->textOffset, relocation);
    relocation->textOffset += _outputSize;
    _relativeRelocations.add(relocation);
}


}