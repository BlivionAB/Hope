#ifndef ELET_COMPILERBASELINEPARSER_H
#define ELET_COMPILERBASELINEPARSER_H


#include <Foundation/Utf8String.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Instruction/ObjectFileWriter/BaselineObjectFileWriter.h>
#include "./X86/X86Parser.h"
#include "./X86/X86BaselinePrinter.h"


using namespace elet::foundation;


namespace elet::domain::compiler::test
{


class CompilerBaselineParser
{

public:

    CompilerBaselineParser(List<uint8_t>& list);

    Utf8String
    write();

private:

    void
    parseSegment();

    List<uint8_t>&
    _list;

    uint64_t
    _offset;

    X86Parser*
    _x86Parser;

    X86BaselinePrinter*
    _x86BaselinePrinter;

    void
    parseTextSection(const Section64* section64);
};


}


#endif //ELET_COMPILERBASELINEPARSER_H
