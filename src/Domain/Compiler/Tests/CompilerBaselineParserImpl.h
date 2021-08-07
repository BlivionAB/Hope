#include "CompilerBaselineParser.h"
#include "../Instruction/ObjectFileWriter/MachoFileWriter.h"
#include "./X86/X86AssemblyParser.h"
#include "./Aarch/Aarch64AssemblyParser.h"

namespace elet::domain::compiler::test
{



template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::CompilerBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget) :
    _list(list),
    _parser(new TAssemblyParser()),
    _baselinePrinter(new TAssemblyPrinter()),
    _assemblyTarget(assemblyTarget)
{
    _baselinePrinter->textSectionInstructions = &textSectionInstructions;
    _baselinePrinter->stubsSectionInstructions = &stubsSectionInstructions;
    _baselinePrinter->stubHelperSectionInstructions = &stubHelperSectionInstructions;
}


template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
void
CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parse()
{
    _baselinePrinter->list = &_list;
    MachHeader64* machHeader = reinterpret_cast<MachHeader64*>(&_list[0]);
    if (machHeader->magic != MACHO_MAGIC_64)
    {
        throw std::runtime_error("machHeader magic is wrong.");
    }
    _offset = sizeof(MachHeader64);
    for (size_t i = 0; i < machHeader->numberOfCommands; ++i)
    {
        Command* cmd = reinterpret_cast<Command*>(&_list[_offset]);
        switch (cmd->cmd)
        {
            case LcSegment64:
                parseSegment64();
                break;
            case LcSymbtab:
                parseSymbolTable(reinterpret_cast<SymtabCommand*>(cmd));
                break;
            default:
                _offset += cmd->cmdSize;
        }
    }
}


template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
void
CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parseSegment64()
{
    SegmentCommand64* segment = reinterpret_cast<SegmentCommand64*>(&_list[_offset]);
    _offset += sizeof(SegmentCommand64);
    for (size_t n = 0; n < segment->numberOfSections; ++n)
    {
        Section64* section = reinterpret_cast<Section64*>(&_list[_offset]);
        if (std::strcmp(section->sectionName, "__text") == 0)
        {
            parseTextSection(section, textSectionInstructions);
            _baselinePrinter->textSectionStartAddress = section->offset;
        }
        else if (std::strcmp(section->sectionName, "__stubs") == 0)
        {
            parseTextSection(section, stubsSectionInstructions);
        }
        else if (std::strcmp(section->sectionName, "__stub_helper") == 0)
        {
            parseTextSection(section, stubHelperSectionInstructions);
        }
        else if (std::strcmp(section->sectionName, "__cstring") == 0)
        {
            _baselinePrinter->cstringSectionOffset = section->offset;
            _baselinePrinter->cstringSectionSize = section->size;
        }
        _offset += sizeof(Section64);
    }
}


template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
template<typename T>
void
CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parseTextSection(const Section64* section, List<T>& instructions)
{
    _parser->parse(instructions, _list, section->offset, section->size);
}


template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
void
CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parseSymbolTable(const SymtabCommand* command)
{
    _offset += sizeof(SymtabCommand);
    SymbolTableEntry* entry = reinterpret_cast<SymbolTableEntry*>(&_list[command->symbolOffset]);
    const char* stringTable = reinterpret_cast<const char*>(&_list[command->stringOffset]);
    for (int i = 0; i < command->symbolEntries; ++i)
    {
        SymbolTableEntry* e = entry + i;
        if (e->sectionIndex != 0)
        {
            _symbols[e->value] = &stringTable[e->stringTableIndex];
        }
    }
    _baselinePrinter->symbols = &_symbols;
}


template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
Utf8String
CompilerBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::serializeTextSegment()
{
    return _baselinePrinter->print();
}


}