#ifndef ELET_COMPILERBASELINEPARSER_H
#define ELET_COMPILERBASELINEPARSER_H


#include <Foundation/Utf8String.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Instruction/ObjectFileWriter/MachoFileWriter.h>
#include "./X86/X86AssemblyParser.h"
#include "./X86/X86AssemblyPrinter.h"
#include "./Aarch/Aarch64AssemblyParser.h"
#include "./Aarch/Aarch64AssemblyPrinter.h"
#include "./BaselinePrinter.h"


using namespace elet::foundation;


namespace elet::domain::compiler::test
{


using namespace elet::domain::compiler::test::aarch;


template<
    typename TAssemblyParser,
    typename TAssemblyPrinter,
    typename TOneOfInstructions
>
class CompilerBaselineParser
{

public:

    CompilerBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget);

    void
    parse();

    List<TOneOfInstructions>
    textSectionInstructions;

    List<TOneOfInstructions>
    stubsSectionInstructions;

    List<TOneOfInstructions>
    stubHelperSectionInstructions;

    Utf8String
    serializeTextSegment();

private:

    void
    parseSegment64();

    List<uint8_t>&
    _list;

    uint64_t
    _offset;

    TAssemblyParser*
    _parser;

    TAssemblyPrinter*
    _baselinePrinter;

    AssemblyTarget
    _assemblyTarget;

    Aarch64AssemblyPrinter*
    _aarch64BaselinePrinter;

    x86::X86AssemblyPrinter*
    _x86BaselinePrinter;

    std::map<uint64_t, const char*>
    _symbols;

    template<typename T = TOneOfInstructions>
    void
    parseTextSection(const Section64* section, List<T>& instructions);

    void
    parseSymbolTable(const SymtabCommand* command);
};



}

#include "CompilerBaselineParserImpl.h"

#endif //ELET_COMPILERBASELINEPARSER_H
