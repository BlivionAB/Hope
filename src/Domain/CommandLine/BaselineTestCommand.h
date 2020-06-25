#ifndef ELET_BASELINETESTCOMMAND_H
#define ELET_BASELINETESTCOMMAND_H

#include <map>

const char* HELP_TEXT =
"This text is pretty long, but will be "
"concatenated into just a single string. "
"The disadvantage is that you have to quote "
"each part, and newlines must be literal as "
"usual.";

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
