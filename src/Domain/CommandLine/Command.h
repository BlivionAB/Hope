//
// Created by Tien Heng Ho on 2020-06-23.
//

#ifndef ELET_COMMAND_H
#define ELET_COMMAND_H


template<typename TPrinter>
class Command
{
public:
    Command();

    int
    execute(int argc, char** argv);

private:

    TPrinter
    _printer;
};


#endif //ELET_COMMAND_H
