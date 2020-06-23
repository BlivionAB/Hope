#include <iostream>
#include <Domain/CommandLine/Command.h>

int
main(int argc, char *argv[])
{
    Command<> command();
    command.execute(argc, argv);
    return 0;
}
