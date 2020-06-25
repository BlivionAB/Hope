#ifndef ELET_PRINTER_H
#define ELET_PRINTER_H

#include <string>

class Printer
{
public:

    void
    print(const std::string& text) const;

    void
    print(const std::string& text, const std::string& arg1) const;

    void
    print(const std::string& text, const std::string& arg1, const std::string& arg2) const;
};


#endif //ELET_PRINTER_H
