#ifndef ELET_BASELINETESTCOMMAND_H
#define ELET_BASELINETESTCOMMAND_H

#include <map>
#include <Domain/Compiler/Compiler.h>
#include <Foundation/TestRunner.h>
#include <Foundation/CommandLine.h>


using namespace elet::domain::compiler;


struct BaselineTestSelection : public TestSelection
{
    AssemblyTarget
    assembly;
};


static const std::map<const char*, AssemblyTarget, CompareStr> stringToTarget = {
    { "baseline", AssemblyTarget::Baseline },
    { "x86_64", AssemblyTarget::x86_64 },
};

enum class CommandType
{
    Root,
    Run,
    Accept,
};


static const std::map<CommandType, const char*> commandTypeToString =
{
    { CommandType::Run, "run" },
    { CommandType::Accept, "accept" }
};


enum class FlagType
{
    Help,
    TestSelection,
    FolderSelection,
    AssemblySelection,
};


template<typename TPrinter, typename TTestRunner>
class BaselineTestCommand
{
public:

    BaselineTestCommand(TPrinter& printer, TTestRunner& testRunner);

    int
    execute(int argc, char** argv);

private:

    TTestRunner&
    _testRunner;

    TPrinter&
    _printer;
};

#include "BaselineTestCommandImpl.h"

#endif //ELET_BASELINETESTCOMMAND_H
