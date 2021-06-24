#include "CompilerBaselineParser.h"
#include "../Instruction/ObjectFileWriter/BaselineObjectFileWriter.h"
#include "./X86/X86Parser.h"

namespace elet::domain::compiler::test
{



CompilerBaselineParser::CompilerBaselineParser(List<uint8_t>& list):
    _list(list),
    _x86Parser(new X86Parser()),
    _x86BaselinePrinter(new X86BaselinePrinter())
{

}

Utf8String
CompilerBaselineParser::write()
{
    _x86BaselinePrinter->list = &_list;
    MachHeader64* machHeader = reinterpret_cast<MachHeader64*>(&_list[0]);
    if (machHeader->magic != MACHO_MAGIC_64)
    {
        throw std::runtime_error("machHeader magic is wrong.");
    }
    if (machHeader->cpuType != CPU_TYPE_X86_64)
    {
        throw std::runtime_error("cpuType does not correspond to CPU_TYPE_X86_64.");
    }
    if (machHeader->cpuSubType != CPU_SUBTYPE_X86_64_ALL)
    {
        throw std::runtime_error("cpuSubType does not correspond to CPU_SUBTYPE_X86_64_ALL.");
    }
    _offset = sizeof(MachHeader64);
    for (size_t i = 0; i < machHeader->numberOfCommands; ++i)
    {
        Command* cmd = reinterpret_cast<Command*>(&_list[_offset]);
        switch (cmd->cmd)
        {
            case LcSegment64:
                parseSegment();
                break;
            case LcSymbtab:
                parseSymbolTable(reinterpret_cast<SymtabCommand*>(cmd));
                break;
            default:
                _offset += cmd->cmdSize;
        }
    }
    return _x86BaselinePrinter->print();
}


void
CompilerBaselineParser::parseSegment()
{
    SegmentCommand64* segment = reinterpret_cast<SegmentCommand64*>(&_list[_offset]);
    _offset += sizeof(SegmentCommand64);
    for (size_t n = 0; n < segment->numberOfSections; ++n)
    {
        Section64* section = reinterpret_cast<Section64*>(&_list[_offset]);
        if (std::strcmp(section->sectionName, "__text") == 0)
        {
            parseTextSection(section);
        }
        else if (std::strcmp(section->sectionName, "__cstring") == 0)
        {
            _x86BaselinePrinter->cstringSectionOffset = section->offset;
            _x86BaselinePrinter->cstringSectionSize = section->size;
        }
        _offset += sizeof(Section64);
    }
}


void
CompilerBaselineParser::parseTextSection(const Section64* textSection)
{
    _x86BaselinePrinter->instructions = _x86Parser->parse(_list, textSection->offset, textSection->size);
    _x86BaselinePrinter->address = textSection->offset;
}


void
CompilerBaselineParser::parseSymbolTable(const SymtabCommand* command)
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
    _x86BaselinePrinter->symbols = &_symbols;
}

}