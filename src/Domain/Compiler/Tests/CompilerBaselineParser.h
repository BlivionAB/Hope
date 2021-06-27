#ifndef ELET_COMPILERBASELINEPARSER_H
#define ELET_COMPILERBASELINEPARSER_H


#include <Foundation/Utf8String.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Instruction/ObjectFileWriter/MachoFileWriter.h>
#include "./X86/X86Parser.h"
#include "./X86/X86BaselinePrinter.h"
#include "./Aarch/Aarch64Parser.h"
#include "./Aarch/Aarch64BaselinePrinter.h"
#include "./BaselinePrinter.h"


using namespace elet::foundation;


namespace elet::domain::compiler::test
{


using namespace elet::domain::compiler::test::aarch;


class CompilerBaselineParser
{

public:

    CompilerBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget);

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

    AssemblyTarget
    _assemblyTarget;

    AssemblyParser*
    _assemblyParser;

    Aarch64Parser*
    _aarch64Parser;

    Aarch64BaselinePrinter*
    _aarch64BaselinePrinter;

    X86BaselinePrinter*
    _x86BaselinePrinter;

    BaselinePrinter*
    _baselinePrinter;

    std::map<uint64_t, const char*>
    _symbols;

    void
    parseTextSection(const Section64* section64);

    void
    parseSymbolTable(const SymtabCommand* command);
};


}


#endif //ELET_COMPILERBASELINEPARSER_H
