#include <map>
#include <cmath>
//#include <Domain/Instruction/Instruction.h>
#include "MachOFileWriter.h"

namespace elet::domain::compiler::instruction::output
{


void
MachOFileWriter::writeToFile(const Path &file, const AssemblySegments& segments)
{
    std::ofstream outputFileStream(file.toString().asString(), std::ios::out | std::ios::binary);
    _outputFileStream = &outputFileStream;
    layoutCommands(segments);
    writeCommands(segments);
    _outputFileStream->close();
}


void
MachOFileWriter::writeCommands(const AssemblySegments &segments)
{
    writeHeader();
    writeSegmentCommand();
    writeSectionCommands();
    writeSymbolTableCommand();
    writeSectionData();
    writeRelocations(segments.symbolicRelocations);
    writeSymbols(segments);
}


void
MachOFileWriter::writeSymbolTableCommand()
{
//    _symbolTableCommand.stringTableOffset = _commandEndOffset + _dataOffset + _symbolTableCommand.commandSize;
//    _symbolTableCommand.stringTableSize = 0;
    write<SymbolTableCommand>(_symbolTableCommand);
}


void
MachOFileWriter::layoutCommands(const AssemblySegments& segments)
{
    layoutSegmentCommand();
    layoutSymbolTableCommand(segments);
    layoutSection("__text", "__TEXT", SectionDataType::Assembly, segments.text, segments.textSize, 4, (S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS));
    layoutSection("__cstring", "__TEXT", SectionDataType::CString, segments.cstrings, segments.cstringSize, 1, S_CSTRING_LITERALS);
    layoutDataOffsetInSections();
    layoutRelocations(segments.symbolicRelocations);
    auto symbolOffset = _commandEndOffset + _dataOffset;
    _symbolTableCommand.symbolTableOffset = symbolOffset;
    auto stringTableOffset = segments.symbols->size() * sizeof(SymbolEntry64);
    _symbolTableCommand.stringTableOffset = _symbolTableCommand.symbolTableOffset + stringTableOffset;
    _symbolTableCommand.stringTableSize = segments.symbolSize;
    _segmentCommand.fileOffset = _commandEndOffset;
}


void
MachOFileWriter::layoutRelocations(const List<RelocationOperand*>* relocations)
{
    auto textSection = _layoutedSections[0]->definition;
    textSection->relocationOffset = _commandEndOffset + _dataOffset;
    auto size = textSection->numberOfRelocations = relocations->size();
    _dataOffset += size * sizeof(RelocationInfo);
}


void
MachOFileWriter::writeHeader()
{
    write<MachHeader64>(_header);
}


void
MachOFileWriter::writeSegmentCommand()
{
    _segmentCommand.fileOffset = _commandEndOffset;
    write<SegmentCommand64>(_segmentCommand);
}


void
MachOFileWriter::layoutDataOffsetInSections()
{
    for (const auto sectionData : _layoutedSections)
    {
        sectionData->definition->offset = _commandEndOffset + _dataOffset;
        _dataOffset += sectionData->size + sectionData->fillBytes;
    }
}


void
MachOFileWriter::writeSectionCommands()
{
    for (const auto sectionData : _layoutedSections)
    {
        write<Section64>(*sectionData->definition);
    }
}


void
MachOFileWriter::writeSectionData()
{
    for (const auto sectionData : _layoutedSections)
    {
        switch (sectionData->dataType)
        {
            case SectionDataType::Assembly:
            {

                auto routines = reinterpret_cast<const List<Routine*>*>(sectionData->data);
                for (const auto routine : *routines)
                {
                    auto instructions = routine->machineInstructions;
                    auto size = instructions->size();
                    _outputFileStream->write((char*)&(*instructions)[0], size);
                }
                fillWithNullCharacter(sectionData->fillBytes);
                break;
            }
            case SectionDataType::CString:
            {
                auto strings = reinterpret_cast<const List<Utf8StringView*>*>(sectionData->data);
                for (const Utf8StringView* string : *strings)
                {
                    auto size = string->size();
                    _outputFileStream->write(string->source(), size);
                    writeNullCharacter();
                }
                fillWithNullCharacter(sectionData->fillBytes);
                break;
            }
        }
    }
}


void
MachOFileWriter::writeRelocations(const List<RelocationOperand*>* relocations)
{
    for (const auto relocation : *relocations)
    {
        if (relocation->symbol)
        {
            write<RelocationInfo>({
                relocation->textOffset,
                relocation->symbol->index,
                1,
                4,
                1,
                RELOCATION_TYPE_X86_64_BRANCH
            });
        }
    }
}


void 
MachOFileWriter::writeSymbols(const AssemblySegments& segments) 
{
    std::uint32_t offset = 1;
    for (const Symbol* symbol : *segments.symbols)
    {
        SymbolEntry64 s {
            offset,
            N_SECT | N_EXT,
            1,
            0,
            symbol->textOffset,
        };
        write<SymbolEntry64>(s);
        offset += symbol->name.size() + 1;
    }
    writeNullCharacter();
    for (const Symbol* symbol : *segments.symbols)
    {
        writeStringView(symbol->name);
        writeNullCharacter();
    }
}


void
MachOFileWriter::layoutSection(const char *sectionName, const char *segmentName, const SectionDataType dataType, void* data, std::size_t size, std::uint32_t alignment, std::uint32_t flags)
{
    std::array<char, 16> _sectionName {};
    std::memcpy(&_sectionName, sectionName, strlen(sectionName));
    std::array<char, 16> _segmentName {};
    std::memcpy(&_segmentName, segmentName, strlen(sectionName));
    auto section64 = new Section64 {
        _sectionName,
        _segmentName,
        _currentSegmentAddress,
        size,
        0,
        alignment,
        0,
        0,
        flags,
    };
    _currentSegmentAddress += size;
    auto restBytes = size % 8;
    auto fillBytes = restBytes > 0 ? 8 - restBytes : 0;
    auto sectionData = new SectionData (
        section64,
        dataType,
        data,
        size,
        fillBytes
    );
    _commandEndOffset += sizeof(Section64);
    _segmentCommand.commandSize += sizeof(Section64);
    _segmentCommand.numberOfSections++;
    _segmentCommand.virtualMemorySize += size;
    _segmentCommand.fileSize += size;
    _header.sizeOfCommands += sizeof(Section64);
    _layoutedSections.add(sectionData);
}


template<typename T>
void MachOFileWriter::write(const T& data)
{
    _outputFileStream->write((char*)&data, sizeof(data));
    _written += sizeof(data);
}


template<typename T>
void MachOFileWriter::write(const T&& data)
{
    _outputFileStream->write((char*)&data, sizeof(data));
    _written += sizeof(data);
}


void
MachOFileWriter::writeStringView(const Utf8StringView &string)
{
    std::size_t size = string.size();
    _written += size;
    _outputFileStream->write(string.source(), size);
}


void
MachOFileWriter::writeNullCharacter()
{
    _written++;
    _outputFileStream->write("\0", 1);
}


void
MachOFileWriter::fillWithNullCharacter(std::size_t rest)
{
    for (unsigned int i = 0; i < rest; i++)
    {
        writeNullCharacter();
    }
}


void
MachOFileWriter::layoutSegmentCommand()
{
    _header.numberOfCommands++;
    _commandEndOffset += sizeof(SegmentCommand64);
    _header.sizeOfCommands += sizeof(SegmentCommand64);
}


void
MachOFileWriter::layoutSymbolTableCommand(const AssemblySegments& segments)
{
    _symbolTableCommand.numberOfSymbols = segments.symbols->size();
    _commandEndOffset += sizeof(SymbolTableCommand);
    _header.sizeOfCommands += sizeof(SymbolTableCommand);
    _header.numberOfCommands++;
}


}