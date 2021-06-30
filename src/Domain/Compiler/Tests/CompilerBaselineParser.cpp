#include "CompilerBaselineParser.h"
#include "../Instruction/ObjectFileWriter/MachoFileWriter.h"
#include "./X86/X86Parser.h"
#include "./Aarch/Aarch64Parser.h"

namespace elet::domain::compiler::test
{



CompilerBaselineParser::CompilerBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget) :
    _list(list),
    _x86Parser(nullptr),
    _x86BaselinePrinter(nullptr),
    _assemblyTarget(assemblyTarget)
{
    switch (assemblyTarget)
    {
        case AssemblyTarget::x86_64:
            _assemblyParser = _x86Parser = new X86Parser();
            _baselinePrinter = _x86BaselinePrinter = new X86BaselinePrinter();
            break;
        case AssemblyTarget::AArch64:
            _assemblyParser = _aarch64Parser = new Aarch64Parser();
            _baselinePrinter = _aarch64BaselinePrinter = new Aarch64BaselinePrinter();
            break;
        default:
            throw std::runtime_error("Unknown assembly target");
    }
}

Utf8String
CompilerBaselineParser::write()
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
                parseSegment();
                break;
            case LcSymbtab:
                parseSymbolTable(reinterpret_cast<SymtabCommand*>(cmd));
                break;
            default:
                _offset += cmd->cmdSize;
        }
    }
    return _baselinePrinter->print();
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
            _baselinePrinter->cstringSectionOffset = section->offset;
            _baselinePrinter->cstringSectionSize = section->size;
        }
        _offset += sizeof(Section64);
    }
}


void
CompilerBaselineParser::parseTextSection(const Section64* textSection)
{
    switch (_assemblyTarget)
    {
        case AssemblyTarget::AArch64:
            _aarch64Parser->parse(_aarch64BaselinePrinter->instructions, _list, textSection->offset, textSection->size);
            break;
        case AssemblyTarget::x86_64:
            _x86BaselinePrinter->instructions = _x86Parser->parse(_list, textSection->offset, textSection->size);
            break;
        default:
            throw std::runtime_error("Unknown assembly target");
    }
    _baselinePrinter->address = textSection->offset;
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
    _baselinePrinter->symbols = &_symbols;
}

}