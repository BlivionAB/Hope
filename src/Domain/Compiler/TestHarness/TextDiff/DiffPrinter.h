#ifndef ELET_DIFFPRINTER_H
#define ELET_DIFFPRINTER_H


#include <unordered_map>
#include "./MyersDiff.h"
#include <Foundation/TextWriter.h>

namespace elet::domain::compiler::test
{
    enum class TerminalColor
    {
        Red,
        Green,
    };

    class DiffPrinter
    {

    public:

        DiffPrinter();

        std::string
        print(const List <Edit>& diffs);

        std::string
        print(const List <Edit>& diffs, bool printTerminalColors);


    private:

        void
        writeTerminalColor(TerminalColor color);

        void
        writeTerminalColorReset();

        bool
        _writeTerminalColors;

        TextWriter
        _tw;
    };
}


#endif //ELET_DIFFPRINTER_H
