#ifndef ELET_MACHOFILEWRITER_H
#define ELET_MACHOFILEWRITER_H

#include <cstdint>
#include <Foundation/List.h>
#include <fstream>
#include <array>
#include <queue>
#include <mach/machine.h>
#include "ObjectFileWriterInterface.h"

namespace elet::domain::compiler::instruction::output
{


using namespace elet::foundation;

// For reference: https://opensource.apple.com/source/xnu/xnu-1456.1.26/EXTERNAL_HEADERS/mach-o/loader.h

#define MACHO_MAGIC_64 0xfeedfacf /* the 64-bit mach magic number */
#define MACHO_CIGAM_64 0xcffaedfe /* NXSwapInt(MH_MAGIC_64) */

#define	MACHO_OBJECT_FILE_TYPE	    0x1
/* Constants for the cmd field of all load commands, the type */
#define	LC_SEGMENT	0x1	/* segment of this file to be mapped */
#define	LC_SEGMENT_64	0x19
#define	LC_SYMTAB	    0x2

#define CPU_ARCH_MASK           0xff000000      /* mask for architecture bits */
#define CPU_ARCH_ABI64          0x01000000      /* 64 bit ABI */
#define CPU_ARCH_ABI64_32       0x02000000      /* ABI for 64-bit hardware with 32-bit types; LP32 */

#define CPU_TYPE_X86            0x7
#define CPU_TYPE_X86_64 (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_SUBTYPE_X86_64_ALL 0x3

struct MachHeader64
{
    std::uint32_t
    magic;

    std::uint32_t
    cpuType;

    std::uint32_t
    cpuSubType;

    std::uint32_t
    fileType;

    std::uint32_t
    numberOfCommands;

    std::uint32_t
    sizeOfCommands;

    std::uint32_t
    flags;

    std::uint32_t
    reserved;
};


struct SegmentCommand
{
    std::uint32_t
    command;

    std::uint32_t
    commandSize;

    char
    segmentName[16];

    std::uint32_t
    virtualMemoryAddress;

    std::uint32_t
    virtualMemorySize;

    std::uint32_t
    fileOffset;

    std::uint32_t
    fileSize;

    int
    maxProtection;

    int
    initialProtection;

    std::uint32_t
    numberOfSections;

    std::uint32_t
    flags;
};


#define VM_PROTECTION_READ    0x1
#define VM_PROTECTION_WRITE   0x2
#define VM_PROTECTION_EXECUTE 0x4
#define VM_PROTECTION_ALL (VM_PROTECTION_READ | VM_PROTECTION_WRITE | VM_PROTECTION_EXECUTE)


struct SegmentCommand64
{
    std::uint32_t
    command;

    // includes sizeof section_64 structs
    std::uint32_t
    commandSize;

    char
    segmentName[16];

    std::uint64_t
    vmAddress;

    std::uint64_t
    vmSize;

    std::uint64_t
    fileOffset;

    std::uint64_t
    fileSize;

    int
    maximumProtection;

    int
    initialProtection;

    std::uint32_t
    numberOfSections;

    std::uint32_t
    flags;
};



struct Section64
{
    std::array<char, 16>
    sectionName;

    std::array<char, 16>
    segmentName;

    std::uint64_t
    address;

    std::uint64_t
    size;

    std::uint32_t
    offset;

    std::uint32_t
    align;

    std::uint32_t
    relocationOffset;

    std::uint32_t
    numberOfRelocations;

    std::uint32_t
    flags;

    std::uint32_t
    reserved1;

    std::uint32_t
    reserved2;

    std::uint32_t
    reserved3;
};


struct SymbolTableCommand
{
    /* LC_SYMTAB */
    std::uint32_t
    command;

    std::uint32_t
    commandSize;

    /* symbol table offset */
    std::uint32_t
    symbolOffset;

    std::uint32_t
    numberOfSymbols;

    std::uint32_t
    stringTableOffset;

    std::uint32_t
    stringTableSize;
};


struct SymbolEntry64
{
    /* index into the string table */
    std::int32_t
    index;

    /* type flag, see below */
    std::uint8_t
    type;

    /* section number or NO_SECT */
    std::uint8_t
    sectionNumber;

    /* see <mach-o/stab.h> */
    int16_t
    description;

    /* value of this symbol (or stab offset) */
    uint32_t
    value;
};

/*
 * Symbols with a index into the string table of zero (n_un.n_strx == 0) are
 * defined to have a null, "", name.  Therefore all string indexes to non null
 * names must not have a zero string index.  This is bit historical information
 * that has never been well documented.
 */

/*
 * The n_type field really contains four fields:
 *	unsigned char N_STAB:3,
 *		      N_PEXT:1,
 *		      N_TYPE:3,
 *		      N_EXT:1;
 * which are used via the following masks.
 */
#define	N_STAB	0xe0  /* if any of these bits set, a symbolic debugging entry */
#define	N_PEXT	0x10  /* private external symbol bit */
#define	N_TYPE	0x0e  /* mask for the type bits */
#define	N_EXT	0x01  /* external symbol bit, set for external symbols */

static const MachHeader64 DEFAULT_HEADER =
{
    MACHO_MAGIC_64,
    MACHO_OBJECT_FILE_TYPE,
    0,
    0,
    0,
    0
};


struct SectionData
{
    Section64*
    definition;

    std::map<Routine*, List<std::uint8_t>*>*
    data;

    SectionData(Section64* definition, std::map<Routine*, List<std::uint8_t>*>* data):
        definition(definition),
        data(data)
    { }
};

struct SymTab
{
    /* LC_SYMTAB */
    std::uint32_t
    command;

    std::uint32_t
    commandSize;

    std::uint32_t
    symbolTableOffset;

    std::uint32_t
    numberOfSymbols;

    std::uint32_t
    stringTableOffset;

    std::uint32_t
    stringTableSize;
};


class MachOFileWriter : public ObjectFileWriterInterface
{
public:

    MachOFileWriter();

    void
    writeToFile(const Path& file, const AssemblySegments& segments);

private:

    void
    layoutCommands(const AssemblySegments& segments);

    void
    writeHeader();

    void
    writeSegmentCommand();

    void
    writeSections();

    void
    layoutSection(const char* sectionName, const char* segmentName, std::map<Routine*, List<std::uint8_t>*>* data);

    void
    layoutSegmentCommand();

    void
    writeCommands();

    void
    layoutSymtabCommand(List<Utf8StringView>* symbols);

    template<typename T>
    void
    write(T& data);

    template<typename T>
    void
    write(T&& type);

    std::ofstream*
    _outputFileStream;

    std::vector<unsigned char>
    _output;

    MachHeader64
    _header =
    {
        MACHO_MAGIC_64,
        CPU_TYPE_X86_64,
        CPU_SUBTYPE_X86_64_ALL,
        MACHO_OBJECT_FILE_TYPE,
        0,
        0,
        0,
        0
    };

    SegmentCommand64
    _segmentCommand =
    {
        LC_SEGMENT_64,
        0,
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
        0,
        0,
        0,
        0,
        VM_PROTECTION_ALL,
        VM_PROTECTION_ALL,
        2,
        0,
    };

    SymTab
    _symTab =
    {
        LC_SYMTAB,
        sizeof(SymTab),
        0,
        0,
        0,
        0
    };

    std::queue<SectionData*>
    _layoutedSections;

    std::uint32_t
    _fileOffset = 0;
};


}

#endif //ELET_MACHOFILEWRITER_H
