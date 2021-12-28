#ifndef ELET_MACHOBASELINEPARSER_H
#define ELET_MACHOBASELINEPARSER_H


#include <Foundation/Utf8String.h>
#include <Foundation/List.h>
#include <Domain/Compiler/Instruction/ObjectFileWriter/MachoFileWriter.h>
#include "Domain/Compiler/TestHarness/X86/X86AssemblyParser.h"
#include "Domain/Compiler/TestHarness/X86/X86AssemblyPrinter.h"
#include "Domain/Compiler/TestHarness/Aarch/Aarch64AssemblyParser.h"
#include "Domain/Compiler/TestHarness/Aarch/Aarch64AssemblyPrinter.h"
#include "./BaselinePrinter.h"
#include "./ObjectFileBaselineParser.h"


using namespace elet::foundation;


namespace elet::domain::compiler::test
{
    using namespace elet::domain::compiler::test::aarch;


    template<
        typename TAssemblyParser,
        typename TAssemblyBaselinePrinter,
        typename TOneOfInstructions
    >
    class MachOBaselineParser : public ObjectFileBaselineParser<TAssemblyParser, TAssemblyBaselinePrinter>
    {

    public:

        MachOBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget);

        void
        parse() override;

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

        Aarch64AssemblyPrinter*
        _aarch64BaselinePrinter;

        x86::X86AssemblyPrinter*
        _x86BaselinePrinter;

        std::map<uint64_t, const char*>
        _symbols;

        template<typename T = TOneOfInstructions>
        void
        parseTextSection(const macho::Section64* section, List<T>& instructions);

        void
        parseSymbolTable(const macho::SymtabCommand* command);
    };
}

#include "MachOBaselineParserImpl.h"

#endif //ELET_MACHOBASELINEPARSER_H
