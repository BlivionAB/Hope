#ifndef ELET_COMMANDLINE_H
#define ELET_COMMANDLINE_H

#include <map>
#include "Utf8String.h"
#include <fmt/format.h>

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

struct CompareStr
{
    bool operator()(char const *a, char const *b) const
    {
        return std::strcmp(a, b) < 0;
    }
};

template<typename T, typename F>
struct CommandDefinition
{
    T
    type;

    std::map<const char*, CommandFlagDefinition<F>, CompareStr>
    flags;

    std::map<const char*, CommandDefinition*, CompareStr>
    subCommands;
};

template<typename C, typename F>
class CommandLine
{
public:
    CommandLine(int argc, char *argv[], CommandDefinition<C, F> rootCommand);

    bool
    hasFlag(F flag);

    const char*
    getFlagValue(F flag);

    C
    command;

private:

    std::map<F, const char*>
    _flags;

    CommandDefinition<C, F>
    _rootCommand;
};

template<typename TCommandType>
struct UnknownCommandFlag : std::exception
{
    const char*
    flag;

    TCommandType
    command;

    explicit UnknownCommandFlag(
        const char*
        flag,

        TCommandType
        command
    ):
        flag(flag),
        command(command)
    { }

    std::string
    message() const
    {
        if (command == CommandType::Root)
        {
            return fmt::format("Unknown flag '{0}'.", flag);
        }
        auto commandString = commandTypeToString.find(command);
        return fmt::format("Unknown flag '{0}' in command '{1}'.", flag, commandString->second);
    }
};


struct UnknownCommand : std::exception
{
    const char*
    command;

    explicit UnknownCommand(const char* command): command(command)
    { }

    std::string
    message() const
    {
        return fmt::format("Unknown command '{0}'.", command);
    }
};

#include "CommandLineImpl.h"

#endif //ELET_COMMANDLINE_H
