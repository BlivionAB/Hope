#include "DiffPrinter.h"

namespace elet::domain::compiler::test
{
    DiffPrinter::DiffPrinter()
    {
        _tw.addColumn(2);
        _tw.addColumn(7);
        _tw.addColumn(12);
    }


    std::string
    DiffPrinter::print(const List<Edit>& diffs)
    {
        return print(diffs, false);
    }

    std::string
    DiffPrinter::print(const List<Edit>& diffs, bool printTerminalColors)
    {
        _writeTerminalColors = printTerminalColors;
        if (printTerminalColors)
        {
            _tw.newline();
            _tw.newline();
        }
        for (const Edit& edit : diffs)
        {
            switch (edit.type)
            {
                case EditType::Equal:
                    _tw.tab();
                    _tw.writeUint(edit.oldLine->number);
                    _tw.tab();
                    _tw.writeUint(edit.newLine->number);
                    _tw.tab();
                    _tw.write(edit.newLine->content);
                    _tw.newline();
                    break;
                case EditType::Delete:
                    writeTerminalColor(TerminalColor::Red);
                    _tw.write("-");
                    _tw.tab();
                    _tw.writeUint(edit.oldLine->number);
                    _tw.tab();
                    _tw.tab();
                    _tw.write(edit.oldLine->content);
                    writeTerminalColorReset();
                    _tw.newline();
                    break;
                case EditType::Insert:
                    writeTerminalColor(TerminalColor::Green);
                    _tw.write("+");
                    _tw.tab();
                    _tw.tab();
                    _tw.writeUint(edit.newLine->number);
                    _tw.tab();
                    _tw.write(edit.newLine->content);
                    writeTerminalColorReset();
                    _tw.newline();
                    break;
                default:
                    throw std::runtime_error("Unknown EditType.");
            }
        }

        return _tw.toString().toString();
    }


    void
    DiffPrinter::writeTerminalColor(TerminalColor color)
    {
        if (_writeTerminalColors)
        {
            switch (color)
            {
                case TerminalColor::Red:
                    _tw.writeZeroLength("\u001b[31m");
                    break;
                case TerminalColor::Green:
                    _tw.writeZeroLength("\u001b[32m");
                    break;
                default:
                    throw std::runtime_error("Unknown terminal color.");
            }
        }
    }


    void
    DiffPrinter::writeTerminalColorReset()
    {
        if (_writeTerminalColors)
        {
            _tw.writeZeroLength("\u001b[0m");
        }
    }
}