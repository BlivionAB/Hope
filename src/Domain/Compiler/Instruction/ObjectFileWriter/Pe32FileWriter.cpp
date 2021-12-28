#include "Pe32FileWriter.h"
#include <Foundation/Crc32.h>
#include <chrono>


namespace elet::domain::compiler::instruction::output::pe32
{
    Pe32FileWriter::Pe32FileWriter(AssemblyTarget assemblyTarget):
        ObjectFileWriter(assemblyTarget)
    {

    }


    List<uint8_t>*
    Pe32FileWriter::write(FunctionRoutine* startRoutine)
    {
        layoutTextSegment(startRoutine);

        writeImageFileHeader();
        writeTextSectionHeader();
        writeDataSectionHeader();
//        writeBssSectionHeader();
        writeDrectveSectionHeader();

        writeSections();
        writeSymbolTable();
        writeStringTable();
        return assemblyWriter->getOutput();
    }

    void
    Pe32FileWriter::writeSections()
    {
        writeTextSection();
        writeDataSection();
        writeDrectveSection();
    }


    void
    Pe32FileWriter::writeDrectveSection()
    {
        _drectveSectionHeader->pointerToRawData = offset;
        Utf8String directives = "   /FAILIFMISMATCH:\"_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\" /DEFAULTLIB:\"MSVCRTD\" /DEFAULTLIB:\"OLDNAMES\"";
        _bw->writeString(directives);
        _drectveSectionHeader->sizeOfRawData = offset - _drectveSectionHeader->pointerToRawData;
    }


    void
    Pe32FileWriter::writeImageFileHeader()
    {
        const auto p1 = std::chrono::system_clock::now();
        uint32_t now = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

        ImageFileHeader imageFileHeader =
        {
            .machine = static_cast<uint16_t>(MachineType::IMAGE_FILE_MACHINE_AMD64),
            .numberOfSections = 0,
            .timeDateStamp = now,
            .pointerToSymbolTable = 0,
            .numberOfSymbols = 0,
            .sizeOfOptionalHeader = 0,
            .characteristics = 0,
        };
        _imageFileHeader = output.write(&imageFileHeader);
        offset += sizeof(ImageFileHeader);
    }


    uint32_t calculateChecksum(const List<uint8_t>& data)
    {
        Crc32 crc32(0);
        crc32.update(data);
        return crc32.value();
    }


    void
    Pe32FileWriter::writeTextSectionHeader()
    {
        SectionHeader textSectionHeader =
        {
            .name = ".text",
            .misc = 0,
            .virtualAddress = 0,
            .sizeOfRawData = 0,
            .pointerToRawData = 0,
            .pointerToRelocations = 0,
            .pointerToLineNumbers = 0,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .characteristics = static_cast<uint32_t>(SectionFlag::IMAGE_SCN_CNT_CODE)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_ALIGN_16BYTES)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_EXECUTE)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_READ)
        };
        _textSectionHeader = output.write<SectionHeader>(&textSectionHeader);
        _imageFileHeader->numberOfSections += 1;
        offset += sizeof(SectionHeader);
    }


    void
    Pe32FileWriter::writeDataSectionHeader()
    {
        SectionHeader dataSectionHeader =
        {
            .name = ".data",
            .misc = 0,
            .virtualAddress = 0,
            .sizeOfRawData = 0,
            .pointerToRawData = 0,
            .pointerToRelocations = 0,
            .pointerToLineNumbers = 0,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .characteristics = static_cast<uint32_t>(SectionFlag::IMAGE_SCN_CNT_INITIALIZED_DATA)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_ALIGN_4BYTES)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_WRITE)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_READ)
        };
        _dataSectionHeader = output.write(&dataSectionHeader);
        _imageFileHeader->numberOfSections += 1;
        offset += sizeof(SectionHeader);
    }


    void
    Pe32FileWriter::writeBssSectionHeader()
    {
        SectionHeader bssSectionHeader =
        {
            .name = ".bss",
            .misc = 0,
            .virtualAddress = 0,
            .sizeOfRawData = 0,
            .pointerToRawData = 0,
            .pointerToRelocations = 0,
            .pointerToLineNumbers = 0,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .characteristics = static_cast<uint32_t>(SectionFlag::IMAGE_SCN_CNT_UNINITIALIZED_DATA)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_ALIGN_4BYTES)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_WRITE)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_READ)
        };
        _bssSectionHeader = output.write(&bssSectionHeader);
        _imageFileHeader->numberOfSections += 1;
        offset += sizeof(SectionHeader);
    }


    void
    Pe32FileWriter::writeDrectveSectionHeader()
    {
        SectionHeader drectveSectionHeader =
        {
            .name = { '.' , 'd', 'r', 'e', 'c', 't', 'v', 'e' }, // C++ complains missing null character, because with null character it is longer than 8 chars.
            .misc = 0,
            .virtualAddress = 0,
            .sizeOfRawData = 0,
            .pointerToRawData = 0,
            .pointerToRelocations = 0,
            .pointerToLineNumbers = 0,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .characteristics = static_cast<uint32_t>(SectionFlag::IMAGE_SCN_LNK_INFO)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_LNK_REMOVE)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_ALIGN_1BYTES)
        };
        _drectveSectionHeader = output.write(&drectveSectionHeader);
        _imageFileHeader->numberOfSections += 1;
        offset += sizeof(SectionHeader);
    }


    void
    Pe32FileWriter::layoutTextSegment(FunctionRoutine* startRoutine)
    {
        layoutTextSection(startRoutine);
    }


    void
    Pe32FileWriter::layoutTextSection(FunctionRoutine* startRoutine)
    {
        _textOffset = assemblyWriter->getOffset();
        assemblyWriter->writeTextSection(startRoutine);
        _textSize = assemblyWriter->getOffset() - _textOffset;
    }


    void
    Pe32FileWriter::writeTextSection()
    {
        _textSectionHeader->pointerToRawData = offset;
        output.add(_text);
        _textSectionHeader->sizeOfRawData = _textSize;
        offset += _textSize;
    }


    void
    Pe32FileWriter::writeDataSection()
    {
        _dataSectionHeader->pointerToRawData = offset;
        // TODO: Add real data writing here.
        _dataSectionHeader->sizeOfRawData = 0;
        offset += 0;
    }


    void
    Pe32FileWriter::writeSymbolTable()
    {
        uint64_t symbolTableIndex = 0;
        _imageFileHeader->pointerToSymbolTable = offset;
        writeSectionSymbols(symbolTableIndex);
//        for (FunctionRoutine* function : assemblyWriter->internalRoutines)
//        {
//            function->stringTableIndexAddress = offset;
//
//            ++symbolTableIndex;
//        }
        for (FunctionRoutine* internalRoutine : assemblyWriter->internalRoutines)
        {
            internalRoutine->stringTableIndexAddress = offset;

            SymbolRecord sectionSymbol =
            {
                .name = "",
                .value = 0,
                .sectionNumber = 1,
                .type = 0x20 ,
                .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_EXTERNAL),
                .numberOfAuxSymbols = 0,
            };
            if (internalRoutine->isStartFunction)
            {
                std::memcpy(&sectionSymbol.name, "main", 4);
            }
            else
            {
                const char* name = internalRoutine->name.toString();
                std::memcpy(&sectionSymbol.name, name, internalRoutine->name.size());
            }
            output.write<SymbolRecord>(&sectionSymbol, 18);
            ++symbolTableIndex;
        }
        _imageFileHeader->numberOfSymbols = symbolTableIndex;
    }


    void
    Pe32FileWriter::writeStringTable()
    {
        uint64_t stringTableIndex = 0;
        _bw->writeDoubleWord(4);
//        _bw->writeString("helloworld");
//        for (FunctionRoutine* functionRoutine : assemblyWriter->internalRoutines)
//        {
//            _bw->writeDoubleWordAtAddress(stringTableIndex, functionRoutine->stringTableIndexAddress);
//            _bw->writeString(functionRoutine->name);
//            stringTableIndex += functionRoutine->name.size() + 1;
//        }
//        for (ExternalRoutine* externalRoutine : assemblyWriter->externalRoutines)
//        {
//            _bw->writeDoubleWordAtAddress(stringTableIndex, externalRoutine->stringTableIndexAddress);
//            _bw->writeString(externalRoutine->name);
//            stringTableIndex += externalRoutine->name.size() + 1;
//        }
    }


    void
    Pe32FileWriter::writeSectionSymbols(uint64_t& index)
    {
        writeTextSectionSymbolRecord();
        writeDataSectionSymbolRecord();
//        writeBssSectionSymbolRecord();
        writeDrectveSectionSymbolRecord();
        index += 2 * 3;
    }


    void
    Pe32FileWriter::writeTextSectionSymbolRecord()
    {
        SymbolRecord sectionSymbol =
        {
            .name = ".text",
            .value = 0,
            .sectionNumber = 1,
            .type = static_cast<uint16_t>(SymbolType::IMAGE_SYM_TYPE_NULL),
            .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_STATIC),
            .numberOfAuxSymbols = 1,
        };
        output.write<SymbolRecord>(&sectionSymbol, 18);
        offset += 18;

        AuxiliarySymbol_SectionDefinition sectionDefinition =
        {
            .length = _textSectionHeader->sizeOfRawData,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .checksum = calculateChecksum(_text),
            .number = 1,
            .selection = 0,
            .__unused1 = 0,
            .__unused2 = 0,
        };
        output.write<AuxiliarySymbol_SectionDefinition>(&sectionDefinition, 18);
        offset += 18;
    }


    void
    Pe32FileWriter::writeDataSectionSymbolRecord()
    {
        SymbolRecord symbolRecord =
        {
            .name = ".data",
            .value = 0,
            .sectionNumber = 2,
            .type = static_cast<uint16_t>(SymbolType::IMAGE_SYM_TYPE_NULL),
            .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_STATIC),
            .numberOfAuxSymbols = 1,
        };
        output.write<SymbolRecord>(&symbolRecord, 18);
        offset += 18;

        AuxiliarySymbol_SectionDefinition sectionDefinition =
        {
            .length = _dataSectionHeader->sizeOfRawData,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .checksum = 0,
            .number = 2,
            .selection = 0,
            .__unused1 = 0,
            .__unused2 = 0,
        };
        output.write<AuxiliarySymbol_SectionDefinition>(&sectionDefinition, 18);
        offset += 18;
    }


    void
    Pe32FileWriter::writeBssSectionSymbolRecord()
    {
        SymbolRecord sectionSymbol =
        {
            .name = ".bss",
            .value = 0,
            .sectionNumber =3,
            .type = static_cast<uint16_t>(SymbolType::IMAGE_SYM_TYPE_NULL),
            .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_STATIC),
            .numberOfAuxSymbols = 1,
        };
        output.write<SymbolRecord>(&sectionSymbol, 18);
        offset += 18;

        AuxiliarySymbol_SectionDefinition sectionDefinition =
        {
            .length = _bssSectionHeader->sizeOfRawData,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .checksum = 0,
            .number = 3,
            .selection = 0,
            .__unused1 = 0,
            .__unused2 = 0,
        };
        output.write<AuxiliarySymbol_SectionDefinition>(&sectionDefinition, 18);
        offset += 18;
    }


    void
    Pe32FileWriter::writeDrectveSectionSymbolRecord()
    {
        SymbolRecord sectionSymbol =
        {
            .name = { '.', 'd', 'r', 'e', 'c', 't', 'v', 'e' },
            .value = 0,
            .sectionNumber = 3,
            .type = static_cast<uint16_t>(SymbolType::IMAGE_SYM_TYPE_NULL),
            .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_STATIC),
            .numberOfAuxSymbols = 1,
        };
        output.write<SymbolRecord>(&sectionSymbol, 18);
        offset += 18;

        AuxiliarySymbol_SectionDefinition sectionDefinition =
        {
            .length = _drectveSectionHeader->sizeOfRawData,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .checksum = 0,
            .number = 4,
            .selection = 0,
            .__unused1 = 0,
            .__unused2 = 0,
        };
        output.write<AuxiliarySymbol_SectionDefinition>(&sectionDefinition, 18);
        offset += 18;
    }
}