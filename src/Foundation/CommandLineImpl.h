#include "CommandLine.h"

#include <utility>
#include <Domain/CommandLine/BaselineTestCommand.h>
#include "HashTableMap.h"

template<typename C, typename F>
CommandLine<C, F>::CommandLine(int argc, char **argv, CommandDefinition<C, F> rootCommand, const std::map<C, const char*>& stringToCommand):
    _rootCommand(std::move(rootCommand)),
    command(C::Root)
{
    bool captureValue = false;
    F pendingCaptureFlag;
    CommandDefinition<C, F>* currentCommand = &_rootCommand;
    std::map<const char*, CommandFlagDefinition<F>, CompareStr>* currentFlags = &_rootCommand.flags;
    for (int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        if (captureValue)
        {
            _flags.insert(std::pair(pendingCaptureFlag, arg));
        }
        else if (arg[0] == '-')
        {
            auto flag = currentFlags->find(arg);
            if (flag != currentFlags->end())
            {
                if (flag->second.hasValue)
                {
                    pendingCaptureFlag = flag->second.integerType;
                    captureValue = true;
                    continue;
                }
                else
                {
                    _flags.insert(std::pair(flag->second.integerType, nullptr));
                }
            }
            else
            {
                throw UnknownCommandFlag<C>(arg, currentCommand->type, stringToCommand);
            }
        }
        else {
            auto result = currentCommand->subCommands.find(arg);
            if (result != currentCommand->subCommands.end())
            {
                currentCommand = result->second;
                currentFlags = &currentCommand->flags;
            }
            else
            {
                throw UnknownCommand(arg);
            }
        }
        captureValue = false;
    }
    command = currentCommand->type;
}


template<typename C, typename F>
bool
CommandLine<C, F>::hasFlag(F flagType)
{
    return _flags.find(flagType) != _flags.end();
}


template<typename C, typename F>
const char*
CommandLine<C, F>::getFlagValue(F flagType)
{
    auto value = _flags.find(flagType);
    if (value != _flags.end())
    {
        return value->second;
    }
    return nullptr;
}