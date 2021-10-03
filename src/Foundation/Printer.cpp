#include "Printer.h"
#include <iostream>
#include <format>

void
Printer::print(const std::string& text) const
{
    std::cout << text << std::endl;
}

void
Printer::print(const std::string& text, const std::string& arg1) const
{
    std::cout << std::format(text, arg1) << std::endl;
}

void
Printer::print(const std::string& text, const std::string& arg1, const std::string& arg2) const
{
    std::cout << std::format(text, arg1, arg2) << std::endl;
}
