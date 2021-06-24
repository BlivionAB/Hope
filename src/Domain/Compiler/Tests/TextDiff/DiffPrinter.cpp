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
    _tw.newline();
    _tw.newline();
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
                _tw.writeZeroLength("\u001b[31m");
                _tw.write("-");
                _tw.tab();
                _tw.writeUint(edit.oldLine->number);
                _tw.tab();
                _tw.tab();
                _tw.write(edit.oldLine->content);
                _tw.writeZeroLength("\u001b[0m");
                _tw.newline();
                break;
            case EditType::Insert:
                _tw.writeZeroLength("\u001b[32m");
                _tw.write("+");
                _tw.tab();
                _tw.tab();
                _tw.writeUint(edit.newLine->number);
                _tw.tab();
                _tw.write(edit.newLine->content);
                _tw.writeZeroLength("\u001b[0m");
                _tw.newline();
                break;
            default:
                throw std::runtime_error("Unknown EditType.");
        }
    }

    return _tw.toString().toString();
}


}