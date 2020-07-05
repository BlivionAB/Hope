#include "BaselineTestCommand.h"
#include <iostream>
#include <Foundation/CommandLine.h>
#include <Foundation/Tests/Mocks/TestRunnerMock.h>

static CommandDefinition<CommandType, FlagType> runCommand = {
    CommandType::Run,
    {
        { "--help", { FlagType::Help, nullptr, false } },
        { "--test", { FlagType::TestSelection, nullptr, true } }
    },
    { },
};


static CommandDefinition<CommandType, FlagType> acceptCommand = {
    CommandType::Accept,
    {
        { "--help", { FlagType::Help, nullptr, false } }
    },
    { },
};


const CommandDefinition<CommandType, FlagType> rootCommand = {
    CommandType::Root,
    {
        { "--help", { FlagType::Help, nullptr, false } }
    },
    { { "run", &runCommand }, { "accept", &acceptCommand } },
};


template<typename TPrinter, typename TTestRunner>
BaselineTestCommand<TPrinter, TTestRunner>::BaselineTestCommand(TPrinter& printer, TTestRunner& testRunner):
    _testRunner(testRunner),
    _printer(printer)
{

}


template<typename TPrinter, typename TTestRunner>
int
BaselineTestCommand<TPrinter, TTestRunner>::execute(int argc, char** argv)
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
                {
                    if (cli.hasFlag(FlagType::Help))
                    {
                        _printer.print(HELP_TEXT);
                        break;
                    }
                    TestSelection testSelection {
                        cli.getFlagValue(FlagType::TestSelection),
                        cli.getFlagValue(FlagType::FolderSelection)
                    };
                    _testRunner.execute(testSelection);
                    return 0;
                }
                case CommandType::Accept:
                {
                    break;
                }
            }
        }
    }
    catch (const UnknownCommand& ex)
    {
        _printer.print(ex.message());
        return EXIT_FAILURE;
    }
    catch (const UnknownCommandFlag<CommandType>& ex)
    {
        _printer.print(ex.message());
        return EXIT_FAILURE;
    }
}
