#include "Compiler.h"
#include <fcntl.h>
#include <filesystem>
#include <vector>
#include <mutex>
#include "Instruction/ObjectFileWriter/MachoFileWriter.h"
#include "ErrorWriter.h"

using namespace elet::domain::compiler::instruction::output;


namespace elet::domain::compiler
{
    Compiler::Compiler(FileStreamReader& fileStreamReader, CompilerOptions options):
        _fileStreamReader(fileStreamReader),
        _options(options),
        _parser(new ast::Parser(this->files)),
        _binder(new ast::Binder()),
        _errorPrinter(),
        _checker(new ast::Checker(_binder))
    {
        _transformer = new instruction::Transformer(_dataMutex, options);
        _optimizer = new instruction::output::Optimizer(getOptimizerOptions(options));
        _objectFileWriter = new output::MachoFileWriter(options.assemblyTarget);
    }


    void
    Compiler::compileFile(const fs::path& file)
    {
        std::string fileString = file.string();
        auto result = files.find(fileString);
        if (result != files.end())
        {
            return;
        }
        static const std::size_t BUFFER_SIZE = 16*1024;
        _fileStreamReader.openFile(file);
        size_t fileSize = _fileStreamReader.getFileSize();
        _source = new char[fileSize];
        ast::SourceFile* sourceFile = new ast::SourceFile(fileString.c_str(), _source, _source);
        files.insert(std::pair(fileString, sourceFile));
        sourceFile->start = _source;
        _readCursor = const_cast<char*>(_source);
        fs::path currentDirectory(file);
        while (size_t bytesRead = _fileStreamReader.readChunk(_readCursor, BUFFER_SIZE))
        {
            char* endCursor = &_readCursor[bytesRead];
            if (bytesRead == fileSize)
            {
                sourceFile->end = endCursor;

                // Add source files in order to render type baselines
                _sourceFiles.add(sourceFile);

                _parsingWork.push(sourceFile);
                _parsingWorkCondition.notify_one();
                break;
            }
            _readCursor = endCursor;
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
        acceptOptimizationWork();
        acceptAssemblyWritingWork();
        _display_mutex.lock();
    //    std::cout << "thread %d" << std::this_thread::get_id() << std::endl;
    //    std::cout << "cleanup phase"<< std::endl;
        _display_mutex.unlock();

        // We have to notify all conditions due to some awaiting while one thread can finish all work.
        // Note also, parsing work is the only stage where the thread is awaiting for work from the file
        // read.
        _parsingWorkCondition.notify_all();

        _finishedWork = true;
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


    std::queue<output::FunctionRoutine*>
    Compiler::getStashIR()
    {
        return _routines;
    }


    void
    Compiler::acceptParsingWork()
    {
        _parser->symbols = new List<ast::Symbol*>();
        while (_compilationStage == CompilationStage::Parsing)
        {
            std::unique_lock<std::mutex> lock(_parsingWorkMutex);
            _parsingWorkCondition.wait(lock, [&]
            {
                return !_parsingWork.empty() || !_pendingParsingWork.empty() || (_pendingParsingTasks == 0);
            });
            if (_compilationStage != CompilationStage::Parsing)
            {
                break;
            }
            if (!_parsingWork.empty())
            {
                _pendingParsingTasks++;
                ast::SourceFile* sourceFile = _parsingWork.front();
                _parsingWork.pop();
                lock.unlock();
                auto result = _parser->performWork(sourceFile);
                if (!result.syntaxErrors.isEmpty())
                {
                    _syntaxErrors.add(result.syntaxErrors);
                    _compilationStage = CompilationStage::Error;
                    return;
                }
                if (!result.lexicalErrors.isEmpty())
                {
                    _lexicalErrors.add(result.lexicalErrors);
                    _compilationStage = CompilationStage::Error;
                    return;
                }
                pushBindingWork(result.statements);
                _pendingParsingTasks--;
                // Break early, instead of waiting for condition, that might not fire.
                if (_pendingParsingTasks == 0)
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
            for (ast::Symbol* symbol : *_parser->symbols)
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
                    _bindingWork.push(new ast::BindingWork(functionDeclaration));
                }
                else if (declaration->kind == ast::SyntaxKind::DomainDeclaration)
                {
                    auto domain = reinterpret_cast<ast::DomainDeclaration*>(declaration);
                    _bindingWork.push(new ast::BindingWork(domain));
                }
            }
            else if (statement->labels & LABEL__USING_STATEMENT)
            {
                _bindingWork.push(new ast::BindingWork(reinterpret_cast<ast::UsingStatement*>(statement)));
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
            ast::BindingWork* work = _bindingWork.front();
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
                if (!_checker->errors.isEmpty())
                {
                    _syntaxErrors.add(_checker->errors);
                    _compilationStage = CompilationStage::Error;
                    return;
                }
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

            // TEMP: All declarations are right now functions.
            if (routine->isStartFunction)
            {
                _optimizationWork.push(routine);
            }
            _pendingTransformationTasks--;
        }
    }


    void
    Compiler::acceptOptimizationWork()
    {
        while (_compilationStage == CompilationStage::Optimization)
        {
            _optimizationWorkMutex.lock();
            if (_optimizationWork.empty())
            {
                _optimizationWorkMutex.unlock();
                if (_pendingTransformationTasks == 0)
                {
                    _compilationStage = CompilationStage::Writing;
                    break;
                }
                continue;
            }
            _pendingOptimizationTasks++;
            output::FunctionRoutine* functionRoutine = _optimizationWork.front();
            _optimizer->optimizeRoutine(functionRoutine);
        }
    }


    void
    Compiler::acceptAssemblyWritingWork()
    {
        if (_options.assemblyTarget == AssemblyTarget::StashIR)
        {
            return;
        }
        while (_compilationStage == CompilationStage::Writing)
        {
            if (_routines.empty())
            {
                break;
            }
            output::FunctionRoutine* routine = _routines.front();
            try
            {
                _objectFileWriter->write(routine);
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
            _routines.pop();
        }
    }


    List<uint8_t>&
    Compiler::getOutput()
    {
        return _objectFileWriter->getOutput();
    }


    Utf8String
    Compiler::printCompileErrors()
    {
        _errorPrinter.printCompileErrors(_syntaxErrors);
        _errorPrinter.printLexicalErrors(_lexicalErrors);
        return _errorPrinter.toString();
    }


    List<ast::SourceFile*>
    Compiler::getSourceFiles()
    {
        return _sourceFiles;
    }


    const output::Optimizer::Options
    Compiler::getOptimizerOptions(CompilerOptions options)
    {
        if (options.assemblyTarget == AssemblyTarget::x86_64)
        {
            return { .assemblyHasMultiRegisterOperands = true };
        }
        return { .assemblyHasMultiRegisterOperands = false }
    }
}