#include "BaselinePrinter.h"

namespace elet::domain::compiler::test
{

    template<typename TOneOfInstructions>
    void
    BaselinePrinter<TOneOfInstructions>::writeColumnHeader()
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


    template<typename TOneOfInstructions>
    Utf8String
    BaselinePrinter<TOneOfInstructions>::print()
    {
        _tw.addColumn(18);
        _tw.addColumn(40);
        writeColumnHeader();
        _tw.newline();
        _tw.writeLine("__TEXT,__text:");
        writeInstructions(*textSectionInstructions);
        if (stubsSectionInstructions && stubsSectionInstructions->size() > 0)
        {
            _tw.newline();
            _tw.writeLine("__TEXT,__stubs:");
            _tw.newline();
            writeInstructions(*stubsSectionInstructions);

            _tw.newline();
            _tw.writeLine("__TEXT,__stub_helper:");
            _tw.newline();
            writeInstructions(*stubHelperSectionInstructions);
        }
        return _tw.toString();
    }


    template<typename TOneOfInstructions>
    void
    BaselinePrinter<TOneOfInstructions>::writeAddress()
    {
        auto vmAddress = textSectionStartOffset + vmOffset;
        auto result = symbols->find(vmAddress);
        if (result != symbols->end())
        {
            _tw.newline();
            _tw.write(result->second);
            _tw.writeLine(":");
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