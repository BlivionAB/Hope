//C++11
#include <cstdio>
#include <iostream>
#include <windows.h>
#include <cstdint>
#include <deque>
#include <string>
#include <thread>
#include <cassert>
#include "StringBuilder.h"


namespace elet::foundation
{
    StringBuilder::StringBuilder():
        _string("")
    {

    }


    StringBuilder::StringBuilder(std::string string):
        _string(string)
    {

    }


    void
    StringBuilder::append(std::string string)
    {
        _string += string;
    }


    std::string
    StringBuilder::toString()
    {
        return _string;
    }


    void
    replaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        if(from.empty())
        {
            return;
        }
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
}




