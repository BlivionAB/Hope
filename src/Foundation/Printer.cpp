#include "Printer.h"
#include <iostream>
#include <fmt/core.h>

void
Printer::print(const std::string& text) const
{
    std::cout << text << std::endl;
}

void
Printer::print(const std::string& text, const std::string& arg1) const
{
    std::cout << fmt::format(text, arg1) << std::endl;
}

void
Printer::print(const std::string& text, const std::string& arg1, const std::string& arg2) const
{
    std::cout << fmt::format(text, arg1, arg2) << std::endl;
}
