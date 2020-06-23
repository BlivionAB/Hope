#ifndef ELET_COMMANDLINE_H
#define ELET_COMMANDLINE_H

#include <map>
#include "Utf8String.h"

using namespace elet::foundation;

template<typename T>
struct CommandFlagDefinition
{
    T
    type;

    const char*
    defaultValue;

    bool
    hasValue;
};

template<typename T, typename F>
struct CommandDefinition
{
    T
    type;

    std::map<const char*, CommandFlagDefinition<F>>
    flags;

    std::map<const char*, CommandDefinition*>
    subCommands;
};

template<typename C, typename F>
class CommandLine
{
public:
    CommandLine(int argc, char *argv[], CommandDefinition<C, F> rootCommand);

    bool
    hasFlag(F flag);

    C
    command;

private:

    std::map<F, const char*>
    _flags;

    CommandDefinition<C, F>
    _rootCommand;
};

struct UnknownCommandFlag : std::exception
{
    const char*
    flag;

    explicit UnknownCommandFlag(const char* flag): flag(flag)
    { }
};


struct UnknownCommand : std::exception
{
    const char*
    command;

    explicit UnknownCommand(const char* command): command(command)
    { }
};

#include "CommandLineImpl.h"

#endif //ELET_COMMANDLINE_H
