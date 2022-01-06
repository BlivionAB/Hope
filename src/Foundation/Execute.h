#ifndef ELET_EXECUTE_H
#define ELET_EXECUTE_H


#include <string>
#include <thread>
#include <windows.h>
#include <cassert>


namespace elet::foundation
{
    int executeCommand(
        std::string command, //Command Line
        std::string cwd,  //set to '.' for current directory
        std::string& _stdout, //Return List of StdOut
        std::string& _stderr, //Return List of StdErr
        uint32_t& returnCode);    //Return Exit Code
}


#endif //ELET_EXECUTE_H
