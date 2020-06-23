#include "Command.h"
#include <iostream>
#include <Foundation/CommandLine.h>

enum class CommandType
{
    Root,
    Run,
};

enum class FlagType
{
    Help,
};

const char* HELP_TEXT =
"This text is pretty long, but will be "
"concatenated into just a single string. "
"The disadvantage is that you have to quote "
"each part, and newlines must be literal as "
"usual.";

static CommandDefinition<CommandType, FlagType> runCommand = {
    CommandType::Run,
    {
            { "help", { FlagType::Help, nullptr, false } }
    },
    { },
};

const CommandDefinition<CommandType, FlagType> rootCommand = {
    CommandType::Root,
    {
            { "help", { FlagType::Help, nullptr, false } }
    },
    { { "run", &runCommand  } },
};

template<typename T>
int
Command<T>::execute(int argc, char** argv)
{
    try
    {
        CommandLine<CommandType, FlagType> cli(argc, argv, rootCommand);
        if (cli.command == CommandType::Root)
        {
            if (cli.hasFlag(FlagType::Help))
            {
                _printer.print(HELP_TEXT);
                return 0;
            }
        }
        else
        {
            switch (cli.command)
            {
                case CommandType::Run:
                    break;
            }
        }
    }
    catch (const UnknownCommand& ex)
    {
        ex.command;
        return 1;
    }
}