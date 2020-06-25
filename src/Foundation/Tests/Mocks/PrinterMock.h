#ifndef ELET_PRINTERMOCK_H
#define ELET_PRINTERMOCK_H

#include <stack>
#include <iostream>
#include "Foundation/Printer.h"
#include <gmock/gmock.h>

class PrinterMock : public Printer
{
public:
    MOCK_METHOD(void, print, (const std::string& text));
    MOCK_METHOD(void, print, (const std::string& text, const std::string& arg1));
    MOCK_METHOD(void, print, (const std::string& text, const std::string& arg1, const std::string& arg2));
};


#endif //ELET_PRINTERMOCK_H
