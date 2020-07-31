#include <map>
#include <Domain/Instruction/Instruction.h>
#include "MachOFileWriter.h"

namespace elet::domain::compiler::instruction::output
{


MachOFileWriter::MachOFileWriter()
{

}


void
MachOFileWriter::writeToFile(const Path &file, const AssemblySegments& segments)
{
    std::ofstream outputFileStream(file.toString().asString(), std::ios::out | std::ios::binary);
    _outputFileStream = &outputFileStream;
    layoutCommands(segments);
    writeCommands();
    outputFileStream.close();
}


void
MachOFileWriter::writeCommands()
{
    writeHeader();
    writeSegmentCommand();
    writeSections();
}


void
MachOFileWriter::layoutCommands(const AssemblySegments& segments)
{
    layoutSegmentCommand();
    layoutSection("__text", "__TEXT", segments.text);
    layoutSymtabCommand(nullptr);
    _segmentCommand.fileOffset = _fileOffset;
}


void
MachOFileWriter::writeHeader()
{
    write<MachHeader64>(_header);
}


void
MachOFileWriter::writeSegmentCommand()
{
    _segmentCommand.fileOffset = _fileOffset;
    write<SegmentCommand64>(_segmentCommand);
}


void
MachOFileWriter::writeSections()
{
    for (unsigned int i = 0; i < _layoutedSections.size(); i++)
    {
        SectionData* sectionData = _layoutedSections.front();
        write<Section64>(*sectionData->definition);
    }
}


void
MachOFileWriter::layoutSection(const char* sectionName, const char* segmentName, std::map<Routine*, List<std::uint8_t>*>* data)
{
    std::array<char, 16> _sectionName {};
    std::memcpy(&_sectionName, sectionName, strlen(sectionName));
    std::array<char, 16> _segmentName {};
    std::memcpy(&_segmentName, segmentName, strlen(sectionName));
    auto section64 = new Section64 {
        _sectionName,
        _segmentName,
        0,
        data->size(),
        0,
        1,
        0,
        1,
    };
    auto sectionData = new SectionData {
        section64,
        data,
    };
    _fileOffset += sizeof(Section64);
    _segmentCommand.commandSize += sizeof(Section64);
    _segmentCommand.numberOfSections++;
    _segmentCommand.fileSize += data->size();
    _header.sizeOfCommands += sizeof(Section64);
    _layoutedSections.push(sectionData);
}


template<typename T>
void MachOFileWriter::write(T& data)
{
    _outputFileStream->write((char*)&data, sizeof(data));
    _fileOffset += sizeof(sizeof(data));
}


template<typename T>
void MachOFileWriter::write(T&& some)
{
    auto ptr = reinterpret_cast<unsigned char*>(&some);
    _output.insert(_output.end(), ptr, ptr + sizeof(some));
}


void
MachOFileWriter::layoutSegmentCommand()
{
    _fileOffset += sizeof(SegmentCommand64);
    _header.sizeOfCommands += sizeof(SegmentCommand64);
    _header.numberOfCommands++;
}


void
MachOFileWriter::layoutSymtabCommand(List<Utf8StringView>* symbols)
{
    _fileOffset += sizeof(SymTab);
    _header.sizeOfCommands += sizeof(SymTab);
    _header.numberOfCommands++;
}


}