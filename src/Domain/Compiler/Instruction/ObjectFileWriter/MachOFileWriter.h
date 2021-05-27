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
#define	MH_EXECUTE	                0x2
#define MH_NOUNDEFS                 0x00000001
#define MH_DYLDLINK                 0x00000004
#define MH_TWOLEVEL                 0x00000080
#define MH_PIE                      0x00200000
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

#define S_ATTR_PURE_INSTRUCTIONS 0x80000000
#define S_ATTR_SOME_INSTRUCTIONS 0x00000400
#define S_CSTRING_LITERALS       0x00000002

#define RELOCATION_TYPE_X86_64_UNSIGNED     0x0
#define RELOCATION_TYPE_X86_64_SIGNED       0x1
#define RELOCATION_TYPE_X86_64_BRANCH       0x2
#define RELOCATION_TYPE_X86_64_GOT_LOAD     0x3
#define RELOCATION_TYPE_X86_64_GOT          0x4
#define RELOCATION_TYPE_X86_64_SUBTRACTOR   0x5
#define RELOCATION_TYPE_X86_64_SIGNED1      0x6
#define RELOCATION_TYPE_X86_64_SIGNED2      0x7
#define RELOCATION_TYPE_X86_64_SIGNED4      0x8


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


#define VM_PROTECTION_NONE    0x1
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

/*
 * The following are used to encode rebasing information
 */
#define REBASE_TYPE_POINTER					1
#define REBASE_TYPE_TEXT_ABSOLUTE32				2
#define REBASE_TYPE_TEXT_PCREL32				3

#define REBASE_OPCODE_MASK					0xF0
#define REBASE_IMMEDIATE_MASK					0x0F
#define REBASE_OPCODE_DONE					0x00
#define REBASE_OPCODE_SET_TYPE_IMM				0x10
#define REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB		0x20
#define REBASE_OPCODE_ADD_ADDR_ULEB				0x30
#define REBASE_OPCODE_ADD_ADDR_IMM_SCALED			0x40
#define REBASE_OPCODE_DO_REBASE_IMM_TIMES			0x50
#define REBASE_OPCODE_DO_REBASE_ULEB_TIMES			0x60
#define REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB			0x70
#define REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB	0x80


/*
 * The following are used to encode binding information
 */
#define BIND_TYPE_POINTER					1
#define BIND_TYPE_TEXT_ABSOLUTE32				2
#define BIND_TYPE_TEXT_PCREL32					3

#define BIND_SPECIAL_DYLIB_SELF					 0
#define BIND_SPECIAL_DYLIB_MAIN_EXECUTABLE			-1
#define BIND_SPECIAL_DYLIB_FLAT_LOOKUP				-2

#define BIND_SYMBOL_FLAGS_WEAK_IMPORT				0x1
#define BIND_SYMBOL_FLAGS_NON_WEAK_DEFINITION			0x8

#define BIND_OPCODE_MASK					0xF0
#define BIND_IMMEDIATE_MASK					0x0F
#define BIND_OPCODE_DONE					0x00
#define BIND_OPCODE_SET_DYLIB_ORDINAL_IMM			0x10
#define BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB			0x20
#define BIND_OPCODE_SET_DYLIB_SPECIAL_IMM			0x30
#define BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM		0x40
#define BIND_OPCODE_SET_TYPE_IMM				0x50
#define BIND_OPCODE_SET_ADDEND_SLEB				0x60
#define BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB			0x70
#define BIND_OPCODE_ADD_ADDR_ULEB				0x80
#define BIND_OPCODE_DO_BIND					0x90
#define BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB			0xA0
#define BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED			0xB0
#define BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB		0xC0


/*
 * The following are used on the flags byte of a terminal node
 * in the export information.
 */
#define EXPORT_SYMBOL_FLAGS_KIND_MASK				0x03
#define EXPORT_SYMBOL_FLAGS_KIND_REGULAR			0x00
#define EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL			0x01
#define EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION			0x04
#define EXPORT_SYMBOL_FLAGS_REEXPORT				0x08
#define EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER			0x10

struct DyldInfoCommand
{
    uint32_t
    cmd;

    uint32_t
    cmdSize;

    uint32_t
    rebaseOffset;

    uint32_t
    rebaseSize;

    uint32_t
    bindOffset;

    uint32_t
    bindSize;

    uint32_t
    weakBindOffset;

    uint32_t
    weakBindSize;

    uint32_t
    lazyBindOffset;

    uint32_t
    lazyBindSize;

    uint32_t
    exportOffset;

    uint32_t
    exportSize;
};

struct RelocationInfo
{
    std::uint32_t
    address;

    std::uint32_t
    symbolNumber: 24,
    programCounterRelative: 1,
    length: 2,
    external: 1,
    type: 4;
};


struct SymbolTableCommand
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


struct SymbolEntry64
{
    /* index into the string table */
    std::uint32_t
    index;

    /* type flag, see below */
    std::uint8_t
    type;

    /* section number or NO_SECT */
    std::uint8_t
    sectionIndex;

    /* see <mach-o/stab.h> */
    std::uint16_t
    description;

    /* value of this symbol (or stab offset) */
    std::uint64_t
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
#define	N_STAB	(std::uint8_t)0xe0  /* if any of these bits set, a symbolic debugging entry */
#define	N_PEXT	(std::uint8_t)0x10  /* private external symbol bit */
#define	N_SECT	(std::uint8_t)0x0e  /* mask for the type bits */
#define	N_EXT	(std::uint8_t)0x01  /* external symbol bit, set for external symbols */

static const MachHeader64 DEFAULT_HEADER =
{
    MACHO_MAGIC_64,
    MACHO_OBJECT_FILE_TYPE,
    0,
    0,
    0,
    0
};


enum class SectionDataType : std::uint8_t
{
    Assembly,
    CString,
    Constants,
};


struct SectionData
{
    Section64*
    definition;

    const SectionDataType
    dataType;

    const void*
    data;

    std::size_t
    size;

    std::uint8_t
    fillBytes;

    SectionData(Section64* definition, const SectionDataType dataType, const void* data, std::size_t size, std::uint8_t fillBytes):
        definition(definition),
        dataType(dataType),
        data(data),
        size(size),
        fillBytes(fillBytes)
    { }
};


class MachOFileWriter : public ObjectFileWriterInterface
{
public:

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
    layoutDataOffsetInSections();

    void
    layoutSection(const char *sectionName, const char *segmentName, SectionDataType dataType, void *data, std::size_t size, std::uint32_t alignment, std::uint32_t flags);

    void
    layoutSegmentCommand();

    void
    writeCommands(const AssemblySegments &segments);

    void
    writeSymbolTableCommand();

    void
    writeSectionCommands();

    void
    writeSectionData();

    void
    writeRelocations(const List<RelocationOperand*>* relocations);

    void
    writeSymbols(const AssemblySegments &segments);

    void
    layoutRelocations(const List<RelocationOperand*>* relocations);

    void
    layoutSymbolTableCommand(const AssemblySegments& segments);

    void
    writeStringView(const Utf8StringView& string);

    void
    writeNullCharacter();

    void
    fillWithNullCharacter(std::size_t rest);

    template<typename T>
    void
    write(const T& data);

    template<typename T>
    void
    write(const T&& data);

    std::size_t
    _written;

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
        sizeof(SegmentCommand64),
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
        0,
        0,
        0,
        0,
        VM_PROTECTION_ALL,
        VM_PROTECTION_ALL,
        0,
        0,
    };

    std::uint32_t
    _currentSegmentAddress = 0;

    SymbolTableCommand
    _symbolTableCommand =
    {
        LC_SYMTAB,
        sizeof(SymbolTableCommand),
        0,
        0,
        0,
        0
    };

    List<SectionData*>
    _layoutedSections;

    std::uint32_t
    _commandEndOffset = sizeof(MachHeader64);

    std::uint32_t
    _dataOffset = 0;

    std::uint32_t
    _relocationOffset = 0;
};


}

#endif //ELET_MACHOFILEWRITER_H
