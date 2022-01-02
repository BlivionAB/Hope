#include "Pe32FileWriter.h"
#include <Foundation/Crc32.h>
#include <chrono>


namespace elet::domain::compiler::instruction::output::pe32
{
    Pe32FileWriter::Pe32FileWriter(AssemblyTarget assemblyTarget):
        ObjectFileWriter(assemblyTarget)
    {
        assemblyWriter->setCallingConvention(CallingConvention::MicrosoftX64);
    }


    List<uint8_t>*
    Pe32FileWriter::write(FunctionRoutine* startRoutine)
    {
        layoutTextSegment(startRoutine);

        writeImageFileHeader();
        writeTextSectionHeader();
        writeDataSectionHeader();
        writeRdataSectionHeader();
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
        writeTextSectionRelocations();
        writeDataSection();
        writeRdataSection();
        writeDrectveSection();
    }


    void
    Pe32FileWriter::writeRdataSection()
    {
        _rdataSectionHeader->pointerToRawData = offset;
        List<String*> stringList = assemblyWriter->getStrings();
        uint32_t stringOffset = 0;
        for (String* string : stringList)
        {
            string->relocationAddress.value2 = stringOffset;
            _bw->writeStringWithNullCharEnd(string->value);
            stringOffset += string->value.size() + 1;
        }
        _rdataSectionHeader->sizeOfRawData = offset - _rdataSectionHeader->pointerToRawData;
    }


    void
    Pe32FileWriter::writeTextSectionRelocations()
    {
        _textSectionHeader->pointerToRelocations = offset;
        uint16_t numberOfRelocations = 0;
        List<String*> stringList = assemblyWriter->getStrings();
        for (String* string : stringList)
        {
            _bw->writeDoubleWord(string->relocationAddress.offset);
            string->relocationAddress.value1 = static_cast<uint32_t>(offset);
            _bw->writeDoubleWord(0);
            _bw->writeWord(static_cast<uint16_t>(ImageRelocationType::IMAGE_REL_AMD64_REL32));
            numberOfRelocations++;
        }

        for (ExternalRoutine* externalRoutine : assemblyWriter->externalRoutineList)
        {
            for (uint64_t relocationAddress : externalRoutine->relocationAddressList)
            {
                _bw->writeDoubleWord(relocationAddress);
                externalRoutine->symbolTableIndexRelocationList.add(offset);
                _bw->writeDoubleWord(0);
                _bw->writeWord(static_cast<uint16_t>(ImageRelocationType::IMAGE_REL_AMD64_REL32));
                numberOfRelocations++;
            }
        }
        _textSectionHeader->numberOfRelocations = numberOfRelocations;
    }


    void
    Pe32FileWriter::writeDrectveSection()
    {
        _drectveSectionHeader->pointerToRawData = offset;

        // This follows MSVC build of an object file:
        Utf8String directives = "/FAILIFMISMATCH:\"_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\" /DEFAULTLIB:\"MSVCRTD\" /DEFAULTLIB:\"OLDNAMES\"";

        _bw->writeString(directives);
        _drectveSectionHeader->sizeOfRawData = offset - _drectveSectionHeader->pointerToRawData;
        _drectve = output.slice(_drectveSectionHeader->pointerToRawData, _drectveSectionHeader->sizeOfRawData);
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
    Pe32FileWriter::writeRdataSectionHeader()
    {
        SectionHeader dataSectionHeader =
        {
            .name = ".rdata",
            .misc = 0,
            .virtualAddress = 0,
            .sizeOfRawData = 0,
            .pointerToRawData = 0,
            .pointerToRelocations = 0,
            .pointerToLineNumbers = 0,
            .numberOfRelocations = 0,
            .numberOfLineNumbers = 0,
            .characteristics = static_cast<uint32_t>(SectionFlag::IMAGE_SCN_CNT_INITIALIZED_DATA)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_LNK_COMDAT)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_ALIGN_1BYTES)
                | static_cast<uint32_t>(SectionFlag::IMAGE_SCN_MEM_READ)
        };
        _rdataSectionHeader = output.write(&dataSectionHeader);
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
        output.add(_data);
        _dataSectionHeader->sizeOfRawData = _data.size();
        offset += _data.size();
    }


    void
    Pe32FileWriter::writeSymbolTable()
    {
        uint64_t symbolTableIndex = 0;
        _imageFileHeader->pointerToSymbolTable = offset;
        writeSectionSymbols(symbolTableIndex);

        for (FunctionRoutine* internalRoutine : assemblyWriter->internalRoutineList)
        {
            bool shouldStoreStringInStringTable = internalRoutine->name.size() > 8;
            if (shouldStoreStringInStringTable)
            {
                internalRoutine->stringTableIndexAddress = offset + 4;
            }
            SymbolRecord sectionSymbol =
            {
                .name = "",
                .value = static_cast<uint32_t>(internalRoutine->offset),
                .sectionNumber = 1,
                .type = 0x20,
                .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_EXTERNAL),
                .numberOfAuxSymbols = 0,
            };
            if (!shouldStoreStringInStringTable)
            {
                std::memcpy(&sectionSymbol.name, internalRoutine->name.toString(), internalRoutine->name.size());
            }
            output.write<SymbolRecord>(&sectionSymbol, 18);
            offset += 18;
            ++symbolTableIndex;
        }
        for (ExternalRoutine* externalRoutine : assemblyWriter->externalRoutineList)
        {
            bool shouldStoreStringInStringTable = externalRoutine->name.size() > 8;
            if (shouldStoreStringInStringTable)
            {
                externalRoutine->stringTableIndexAddress = offset + 4;
            }
            SymbolRecord sectionSymbol =
            {
                .name = "",
                .value = 0, // Should be 0 for external function
                .sectionNumber = 0, // Should be 0 for external function
                .type = 0, // This should be zero by looking at MSVC output. Don't know why?
                .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_EXTERNAL),
                .numberOfAuxSymbols = 0,
            };
            output.write<SymbolRecord>(&sectionSymbol, 18);
            offset += 18;
            if (!shouldStoreStringInStringTable)
            {
                std::memcpy(&sectionSymbol.name, externalRoutine->name.toString(), externalRoutine->name.size());
            }
            for (uint64_t relocationOffset : externalRoutine->symbolTableIndexRelocationList)
            {
                _bw->writeDoubleWordAtAddress(symbolTableIndex, relocationOffset);
            }
            ++symbolTableIndex;
        }


        List<String*> stringList = assemblyWriter->getStrings();
        for (String* string : stringList)
        {
            _bw->writeDoubleWordAtAddress(symbolTableIndex, string->relocationAddress.value1);

            bool shouldStoreStringInStringTable = string->value.size() > 8;
            if (shouldStoreStringInStringTable)
            {
                string->relocationAddress.value3 = offset + 4;
            }
            SymbolRecord sectionSymbol =
            {
                .name = "",
                .value = string->relocationAddress.value2,
                .sectionNumber = 3,
                .type = 0, // This should be zero by looking at MSVC output. Don't know why?
                .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_EXTERNAL),
                .numberOfAuxSymbols = 0,
            };
            output.write<SymbolRecord>(&sectionSymbol, 18);
            offset += 18;
            if (!shouldStoreStringInStringTable)
            {
                std::memcpy(&sectionSymbol.name, string->value.toString(), string->value.size());
            }
            _bw->writeDoubleWordAtAddress(symbolTableIndex, string->relocationAddress.value1);
            ++symbolTableIndex;
        }
        _imageFileHeader->numberOfSymbols = symbolTableIndex;
    }


    void
    Pe32FileWriter::writeStringTable()
    {
        uint32_t stringTableIndex = 4;
        uint32_t start = offset;
        _bw->writeDoubleWord(0);
        for (FunctionRoutine* functionRoutine : assemblyWriter->internalRoutineList)
        {
            if (functionRoutine->stringTableIndexAddress == 0)
            {
                continue;
            }
            _bw->writeDoubleWordAtAddress(stringTableIndex, functionRoutine->stringTableIndexAddress);
            _bw->writeStringWithNullCharEnd(functionRoutine->name);
            stringTableIndex += functionRoutine->name.size() + 1;
        }
        for (ExternalRoutine* externalRoutine : assemblyWriter->externalRoutineList)
        {
            if (externalRoutine->stringTableIndexAddress == 0)
            {
                continue;
            }
            _bw->writeDoubleWordAtAddress(stringTableIndex, externalRoutine->stringTableIndexAddress);
            _bw->writeStringWithNullCharEnd(externalRoutine->name);
            stringTableIndex += externalRoutine->name.size() + 1;
        }
        List<String*> stringList = assemblyWriter->getStrings();
        for (String* string : stringList)
        {
            if (string->relocationAddress.value3 == 0)
            {
                continue;
            }
            _bw->writeDoubleWordAtAddress(stringTableIndex, string->relocationAddress.value3);
            _bw->writeStringWithNullCharEnd(string->value);
            stringTableIndex += string->value.size() + 1;
        }
        _bw->writeDoubleWordAtAddress(stringTableIndex, start);
    }


    void
    Pe32FileWriter::writeSectionSymbols(uint64_t& index)
    {
        std::vector<std::string> sectionNameList =
        {
            ".text",
            ".data",
            ".drectve"
        };
        std::vector<const ContainerPtr<SectionHeader, uint8_t>*> sectionHeaderList =
        {
            &_textSectionHeader,
            &_dataSectionHeader,
            &_drectveSectionHeader,
        };
        std::vector<const List<uint8_t>*> sectionData =
        {
            &_text,
            &_data,
            &_drectve,
        };
        uint16_t sectionNumber = 1;
        for (uint16_t i = 0; i < sectionNameList.size(); ++i)
        {
            SymbolRecord sectionSymbol =
            {
                .name = "",
                .value = 0,
                .sectionNumber = static_cast<uint16_t>(i + 1),
                .type = static_cast<uint16_t>(SymbolType::IMAGE_SYM_TYPE_NULL),
                .storageClass = static_cast<uint8_t>(StorageClass::IMAGE_SYM_CLASS_STATIC),
                .numberOfAuxSymbols = 1,
            };
            std::memcpy(&sectionSymbol.name, sectionNameList[i].c_str(), 8);
            output.write<SymbolRecord>(&sectionSymbol, 18);

            AuxiliarySymbol_SectionDefinition sectionDefinition =
            {
                .length = (*sectionHeaderList[i])->sizeOfRawData,
                .numberOfRelocations = 0,
                .numberOfLineNumbers = 0,
                .checksum = calculateChecksum(*sectionData[i]),
                .number = static_cast<uint16_t>(i + 1),
                .selection = 0,
                .__unused1 = 0,
                .__unused2 = 0,
            };
            output.write<AuxiliarySymbol_SectionDefinition>(&sectionDefinition, 18);
            offset += 36;
            sectionNumber++;
            index += 2;
        }
    }
}