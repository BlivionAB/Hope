#ifndef ELET_BASELINEPRINTER_H
#define ELET_BASELINEPRINTER_H


#include <cstdint>
#include <Foundation/Utf8String.h>
#include <Foundation/TextWriter.h>
#include <map>


namespace elet::domain::compiler::test
{


using namespace elet::foundation;

template<typename TOneOfInstruction>
class BaselinePrinter
{

public:

    Utf8String
    print();

    uint64_t
    textSectionStartOffset = 0;

    List<TOneOfInstruction>*
    textSectionInstructions;

    List<TOneOfInstruction>*
    stubsSectionInstructions;

    List<TOneOfInstruction>*
    stubHelperSectionInstructions;

    uint64_t
    cstringSectionOffset;

    uint64_t
    cstringSectionSize;

    std::map<uint64_t, const char*>*
    symbols;

    TextWriter
    _tw;

    List<uint8_t>*
    list;

    uint64_t
    vmOffset = 0x0000000100000000;

protected:

    virtual
    void
    writeInstructions(const List<TOneOfInstruction>& instructions) = 0;

    void
    writeColumnHeader();

    void
    writeAddress();
};

}

#include "BaselinePrinterImpl.h"

#endif //ELET_BASELINEPRINTER_H
