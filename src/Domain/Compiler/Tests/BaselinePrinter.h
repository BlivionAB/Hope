#ifndef ELET_BASELINEPRINTER_H
#define ELET_BASELINEPRINTER_H


#include <cstdint>
#include <Foundation/Utf8String.h>
#include <Domain/Compiler/Instruction/TextWriter.h>
#include <map>


namespace elet::domain::compiler::test
{


using namespace elet::foundation;
using namespace elet::domain::compiler::instruction;

template<typename TOneOfInstruction>
class BaselinePrinter
{

public:

    virtual
    Utf8String
    print(List<TOneOfInstruction>& instructions) = 0;

    uint64_t
    textSectionStartAddress = 0;

    List<TOneOfInstruction>*
    textSectionInstructions;

    List<TOneOfInstruction>*
    stubsSectionInstructions;

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

    void
    writeColumnHeader();

    void
    writeAddress();
};

}

#include "BaselinePrinterImpl.h"

#endif //ELET_BASELINEPRINTER_H
