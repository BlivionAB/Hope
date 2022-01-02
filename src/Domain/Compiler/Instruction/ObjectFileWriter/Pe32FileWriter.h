#ifndef ELET_PE32FILEWRITER_H
#define ELET_PE32FILEWRITER_H


#include "./ObjectFileWriter.h"
#include <array>

namespace elet::domain::compiler::instruction::output::pe32
{

    enum Pe32MagicValue : uint64_t
    {
        MS_MAGIC = 0xfeedfacf,
    };

    enum class MachineType : uint16_t
    {
        IMAGE_FILE_MACHINE_AMD64 = 0x8664,
        IMAGE_FILE_MACHINE_I386 = 0x14c
    };

    struct MsDosHeader
    {
        uint16_t
        e_magic; // Magic number

        uint16_t
        e_cblp; // Bytes on last page of file

        uint16_t
        e_cp; // Pages in file

        uint16_t
        e_crlc; // Relocations

        uint16_t
        e_cparhdr; // Size of header in paragraphs

        uint16_t
        e_minalloc; // Minimum extra paragraphs needed

        uint16_t
        e_maxalloc; // Maximum extra paragraphs needed

        uint16_t
        e_ss; // Initial (relative) SS value

        uint16_t
        e_sp; // Initial SP value

        uint16_t
        e_csum; // Checksum

        uint16_t
        e_ip; // Initial IP value

        uint16_t
        e_cs; // Initial (relative) CS value

        uint16_t
        e_lfarlc; // File address of relocation table

        uint16_t
        e_ovno; // Overlay number

        uint16_t
        e_res[4]; // Reserved words

        uint16_t
        e_oemid; // OEM identifier (for e_oeminfo)

        uint16_t
        e_oeminfo; // OEM information; e_oemid specific

        uint16_t
        e_res2[10]; // Reserved words

        uint32_t
        e_lfanew; // File address of new exe header
    };


    struct ImageFileHeader
    {
        uint16_t
        machine;

        uint16_t
        numberOfSections;

        uint32_t
        timeDateStamp;

        uint32_t
        pointerToSymbolTable;

        uint32_t
        numberOfSymbols;

        uint16_t
        sizeOfOptionalHeader;

        uint16_t
        characteristics;
    };


    enum class SymbolType : uint16_t
    {
        IMAGE_SYM_TYPE_NULL = 0,
        IMAGE_SYM_TYPE_VOID = 1,
        IMAGE_SYM_TYPE_CHAR = 2,
        IMAGE_SYM_TYPE_SHORT = 3,
        IMAGE_SYM_TYPE_INT = 4,
        IMAGE_SYM_TYPE_LONG = 5,
        IMAGE_SYM_TYPE_FLOAT = 6,
        IMAGE_SYM_TYPE_DOUBLE = 7,
        IMAGE_SYM_TYPE_STRUCT = 8,
        IMAGE_SYM_TYPE_UNION = 9,
        IMAGE_SYM_TYPE_ENUM = 10,
        IMAGE_SYM_TYPE_MOE = 11,
        IMAGE_SYM_TYPE_BYTE = 12,
        IMAGE_SYM_TYPE_WORD = 13,
        IMAGE_SYM_TYPE_UINT = 14,
        IMAGE_SYM_TYPE_DWORD = 15,
    };


    enum class SymbolTypeMsb
    {
        IMAGE_SYM_DTYPE_NULL = 0,
        IMAGE_SYM_DTYPE_POINTER = 1,
        IMAGE_SYM_DTYPE_FUNCTION = 2,
        IMAGE_SYM_DTYPE_ARRAY = 3,
    };


    enum class StorageClass : uint8_t
    {
        IMAGE_SYM_CLASS_END_OF_FUNCTION = 0xff,
        IMAGE_SYM_CLASS_NULL = 0,
        IMAGE_SYM_CLASS_AUTOMATIC = 1,
        IMAGE_SYM_CLASS_EXTERNAL = 2,
        IMAGE_SYM_CLASS_STATIC = 3,
        IMAGE_SYM_CLASS_REGISTER = 4,
        IMAGE_SYM_CLASS_EXTERNAL_DEF = 5,
        IMAGE_SYM_CLASS_LABEL = 6,
        IMAGE_SYM_CLASS_UNDEFINED_LABEL = 7,
        IMAGE_SYM_CLASS_MEMBER_OF_STRUCT = 8,
        IMAGE_SYM_CLASS_ARGUMENT = 9,
        IMAGE_SYM_CLASS_STRUCT_TAG = 10,
        IMAGE_SYM_CLASS_MEMBER_OF_UNION = 11,
        IMAGE_SYM_CLASS_UNION_TAG = 12,
        IMAGE_SYM_CLASS_TYPE_DEFINITION = 13,
        IMAGE_SYM_CLASS_UNDEFINED_STATIC = 14,
        IMAGE_SYM_CLASS_ENUM_TAG = 15,
        IMAGE_SYM_CLASS_MEMBER_OF_ENUM = 16,
        IMAGE_SYM_CLASS_REGISTER_PARAM = 17,
        IMAGE_SYM_CLASS_BIT_FIELD = 18,
        IMAGE_SYM_CLASS_BLOCK = 100,
        IMAGE_SYM_CLASS_FUNCTION = 101,
        IMAGE_SYM_CLASS_END_OF_STRUCT = 102,
        IMAGE_SYM_CLASS_FILE = 103,
        IMAGE_SYM_CLASS_SECTION = 104,
        IMAGE_SYM_CLASS_WEAK_EXTERNAL = 105,
        IMAGE_SYM_CLASS_CLR_TOKEN = 106,
    };


    struct SymbolRecord
    {
        char
        name[8];

        uint32_t
        value;

        uint16_t
        sectionNumber;

        uint16_t
        type;

        uint8_t
        storageClass;

        uint8_t
        numberOfAuxSymbols;
    };


    struct AuxiliarySymbol_SectionDefinition
    {
        uint32_t
        length;

        uint16_t
        numberOfRelocations;

        uint16_t
        numberOfLineNumbers;

        uint32_t
        checksum;

        uint16_t
        number;

        uint8_t
        selection;

        uint8_t
        __unused1;

        uint16_t
        __unused2;
    };


    enum class ImageRelocationType : uint16_t
    {
        IMAGE_REL_AMD64_ABSOLUTE = 0x0000,
        IMAGE_REL_AMD64_ADDR64 = 0x0001,
        IMAGE_REL_AMD64_ADDR32 = 0x0002,
        IMAGE_REL_AMD64_ADDR32NB = 0x0003,
        IMAGE_REL_AMD64_REL32 = 0x0004,
    };


    struct ImageRelocation
    {
        uint32_t
        virtualAddress;

        uint32_t
        symbolTableIndex;

        ImageRelocationType
        type;
    };

    enum class SectionFlag : uint32_t
    {
        IMAGE_SCN_TYPE_NO_PAD = 0x00000008,
        IMAGE_SCN_CNT_CODE = 0x00000020,
        IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040,
        IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080,
        IMAGE_SCN_LNK_OTHER = 0x00000100,
        IMAGE_SCN_LNK_INFO = 0x00000200,
        IMAGE_SCN_LNK_REMOVE = 0x00000800,
        IMAGE_SCN_LNK_COMDAT = 0x00001000,
        IMAGE_SCN_GPREL = 0x00008000,
        IMAGE_SCN_MEM_PURGEABLE = 0x00020000,
        IMAGE_SCN_MEM_16BIT = 0x00020000,
        IMAGE_SCN_MEM_LOCKED = 0x00040000,
        IMAGE_SCN_MEM_PRELOAD = 0x00080000,
        IMAGE_SCN_ALIGN_1BYTES = 0x00100000,
        IMAGE_SCN_ALIGN_2BYTES = 0x00200000,
        IMAGE_SCN_ALIGN_4BYTES = 0x00300000,
        IMAGE_SCN_ALIGN_8BYTES = 0x00400000,
        IMAGE_SCN_ALIGN_16BYTES = 0x00500000,
        IMAGE_SCN_ALIGN_32BYTES = 0x00600000,
        IMAGE_SCN_ALIGN_64BYTES = 0x00700000,
        IMAGE_SCN_ALIGN_128BYTES =0x00800000,
        IMAGE_SCN_ALIGN_256BYTES = 0x00900000,
        IMAGE_SCN_ALIGN_512BYTES = 0x00A00000,
        IMAGE_SCN_ALIGN_1024BYTES = 0x00B00000,
        IMAGE_SCN_ALIGN_2048BYTES = 0x00C00000,
        IMAGE_SCN_ALIGN_4096BYTES = 0x00D00000,
        IMAGE_SCN_ALIGN_8192BYTES = 0x00E00000,
        IMAGE_SCN_LNK_NRELOC_OVFL = 0x01000000,
        IMAGE_SCN_MEM_DISCARDABLE = 0x02000000,
        IMAGE_SCN_MEM_NOT_CACHED = 0x04000000,
        IMAGE_SCN_MEM_NOT_PAGED = 0x08000000,
        IMAGE_SCN_MEM_SHARED = 0x10000000,
        IMAGE_SCN_MEM_EXECUTE = 0x20000000,
        IMAGE_SCN_MEM_READ = 0x40000000,
        IMAGE_SCN_MEM_WRITE = 0x80000000,
    };


    struct SectionHeader
    {
        char
        name[8];

        union
        {
            uint32_t
            physicalAddress;

            uint32_t
            virtualSize;
        } misc;

        uint32_t
        virtualAddress;

        uint32_t
        sizeOfRawData;

        uint32_t
        pointerToRawData;

        uint32_t
        pointerToRelocations;

        uint32_t
        pointerToLineNumbers;

        uint16_t
        numberOfRelocations;

        uint16_t
        numberOfLineNumbers;

        uint32_t
        characteristics;
    };


    class Pe32FileWriter : public ObjectFileWriter
    {

    public:

        Pe32FileWriter(AssemblyTarget assemblyTarget);

        List<uint8_t>*
        write(FunctionRoutine* startRoutine) override;

    private:

        ContainerPtr<ImageFileHeader, uint8_t>
        _imageFileHeader;

        uint32_t
        _textOffset;

        uint32_t
        _textSize;

        uint32_t
        _rdataOffset;

        uint32_t
        _rdataSize;

        ContainerPtr<SectionHeader, uint8_t>
        _textSectionHeader;

        ContainerPtr<SectionHeader, uint8_t>
        _dataSectionHeader;

        ContainerPtr<SectionHeader, uint8_t>
        _rdataSectionHeader;

        ContainerPtr<SectionHeader, uint8_t>
        _bssSectionHeader;

        ContainerPtr<SectionHeader, uint8_t>
        _drectveSectionHeader;

        List<uint8_t>
        _data;

        List<uint8_t>
        _drectve;

        void
        writeImageFileHeader();

        void
        writeTextSectionHeader();

        void
        writeDataSectionHeader();

        void
        writeBssSectionHeader();

        void
        writeDrectveSectionHeader();

        void
        layoutTextSection(FunctionRoutine* startRoutine);

        void
        layoutTextSegment(FunctionRoutine* startRoutine);

        void
        writeTextSection();

        void
        writeStringTable();

        void
        writeSymbolTable();

        void
        writeSectionSymbols(uint64_t& index);

        void
        writeTextSectionSymbolRecord();

        void
        writeDataSectionSymbolRecord();

        void
        writeBssSectionSymbolRecord();

        void
        writeDrectveSectionSymbolRecord();

        void
        writeDataSection();

        void
        writeSections();

        void
        writeDrectveSection();

        void
        writeTextSectionRelocations();

        void
        writeRdataSection();

        void
        writeRdataSectionHeader();
    };
}

#endif //ELET_PE32FILEWRITER_H
