#include "MachOBaselineParser.h"
#include "Domain/Compiler/Instruction/ObjectFileWriter/MachoFileWriter.h"
#include "Domain/Compiler/TestHarness/X86/X86AssemblyParser.h"
#include "Domain/Compiler/TestHarness/Aarch/Aarch64Parser.h"

namespace elet::domain::compiler::test
{
    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::MachOBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget):
        ObjectFileBaselineParser<TAssemblyParser, TAssemblyPrinter>(list, assemblyTarget)
    {
        this->_assemblyBaselinePrinter->textSectionInstructions = &textSectionInstructions;
        this->_assemblyBaselinePrinter->stubsSectionInstructions = &stubsSectionInstructions;
        this->_assemblyBaselinePrinter->stubHelperSectionInstructions = &stubHelperSectionInstructions;
    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    void
    MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parse()
    {
        this->_assemblyBaselinePrinter->list = &this->_list;
        macho::MachHeader64* machHeader = reinterpret_cast<macho::MachHeader64*>(&this->_list[0]);
        if (machHeader->magic != static_cast<uint32_t>(macho::MachoMagicValue::MACHO_MAGIC_64))
        {
            throw std::runtime_error("machHeader magic is wrong.");
        }
        this->_offset = sizeof(macho::MachHeader64);
        for (size_t i = 0; i < machHeader->numberOfCommands; ++i)
        {
            macho::Command* cmd = reinterpret_cast<macho::Command*>(&this->_list[this->_offset]);
            switch (cmd->cmd)
            {
                case macho::LcSegment64:
                    parseSegment64();
                    break;
                case macho::LcSymbtab:
                    parseSymbolTable(reinterpret_cast<macho::SymtabCommand*>(cmd));
                    break;
                default:
                    this->_offset += cmd->cmdSize;
            }
        }
    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    void
    MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parseSegment64()
    {
        macho::SegmentCommand64* segment = reinterpret_cast<macho::SegmentCommand64*>(&this->_list[this->_offset]);
        this->_offset += sizeof(macho::SegmentCommand64);
        for (size_t n = 0; n < segment->numberOfSections; ++n)
        {
            macho::Section64* section = reinterpret_cast<macho::Section64*>(&this->_list[this->_offset]);
            if (std::strcmp(section->sectionName, "__text") == 0)
            {
                parseTextSection(section, textSectionInstructions);
                this->_assemblyBaselinePrinter->textSectionStartOffset = section->offset;
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
                this->_assemblyBaselinePrinter->cstringSectionOffset = section->offset;
                this->_assemblyBaselinePrinter->cstringSectionSize = section->size;
            }
            this->_offset += sizeof(macho::Section64);
        }
    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    template<typename T>
    void
    MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parseTextSection(const macho::Section64* section, List<T>& instructions)
    {
        this->_assemblyParser->parse(instructions, this->_list, section->offset, section->size);
    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    void
    MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parseSymbolTable(const macho::SymtabCommand* command)
    {
        this->_offset += sizeof(macho::SymtabCommand);
        macho::SymbolTableEntry* entry = reinterpret_cast<macho::SymbolTableEntry*>(&this->_list[command->symbolOffset]);
        const char* stringTable = reinterpret_cast<const char*>(&this->_list[command->stringOffset]);
        for (int i = 0; i < command->symbolEntries; ++i)
        {
            macho::SymbolTableEntry* e = entry + i;
            if (e->sectionIndex != 0)
            {
                _symbols[e->value] = &stringTable[e->stringTableIndex];
            }
        }
        this->_assemblyBaselinePrinter->symbols = &_symbols;
    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    Utf8String
    MachOBaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::serializeTextSegment()
    {
        return this->_assemblyBaselinePrinter->print();
    }
}