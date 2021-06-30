#include "BaselinePrinter.h"

namespace elet::domain::compiler::test
{

void
BaselinePrinter::writeColumnHeader()
{
    _tw.write("Address");
    _tw.tab();
    _tw.write("Bytes");
    _tw.tab();
    _tw.write("Instruction");
    _tw.newline();
    _tw.write("============================================================");
    _tw.newline();
}



void
BaselinePrinter::writeAddress()
{
    auto vmAddress = address + vmOffset;
    auto result = symbols->find(vmAddress);
    if (result != symbols->end())
    {
        _tw.newline();
        _tw.write(result->second);
        _tw.write(":");
        _tw.newline();
    }
    _tw.writeByteHex((vmAddress >> 56) & 0xff);
    _tw.writeByteHex((vmAddress >> 48) & 0xff);
    _tw.writeByteHex((vmAddress >> 40) & 0xff);
    _tw.writeByteHex((vmAddress >> 32) & 0xff);
    _tw.writeByteHex((vmAddress >> 24) & 0xff);
    _tw.writeByteHex((vmAddress >> 16) & 0xff);
    _tw.writeByteHex((vmAddress >> 8) & 0xff);
    _tw.writeByteHex(vmAddress & 0xff);
}

}