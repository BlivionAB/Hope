#ifndef ELET_BASELINEOBJECTFILEWRITER_H
#define ELET_BASELINEOBJECTFILEWRITER_H

#include "../AssemblyWriter.h"
#include "MachOFileWriter.h"
#include "DyldInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


class DyldInfoWriter;
// For reference: https://opensource.apple.com/source/xnu/xnu-4903.270.47/EXTERNAL_HEADERS/mach-o/loader.h.auto.html
#define SEGMENT_SIZE 0x4000 // 8 byte alignemnt
#define MACHO_MAGIC_64 0xfeedfacf /* the 64-bit mach magic number */
#define MACHO_CIGAM_64 0xcffaedfe /* NXSwapInt(MH_MAGIC_64) */

#define	MACHO_OBJECT_FILE_TYPE	    0x1
/* Constants for the cmd field of all load commands, the type */

enum LoadCommandType : uint8_t
{
    LcSegment64 = 0x19
};

enum CpuArch : uint32_t
{
    // mask for architecture bits
    Mask = 0xff000000,

    // 64 bit ABI
    Abi64 = 0x01000000,

    // ABI for 64-bit hardware with 32-bit types; LP32
    Abi64_32 = 0x02000000,
};

#define CPU_TYPE_X86            0x7
#define CPU_TYPE_X86_64 (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_SUBTYPE_X86_64_ALL 0x3

#define S_ATTR_PURE_INSTRUCTIONS 0x80000000
#define S_ATTR_SOME_INSTRUCTIONS 0x00000400
#define S_CSTRING_LITERALS       0x00000002
#define S_SYMBOL_STUBS           0x00000008



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


enum CommandType : uint32_t
{
    LC_REQ_DYLD = 0x80000000,
    LcSymbtab = 0x2,
    LC_DYSYMTAB = 0xb,
    LC_DYLD_INFO = 0x22,
    LC_DYLD_INFO_ONLY = (LC_DYLD_INFO | LC_REQ_DYLD),
    LC_LOAD_DYLIB = 0xc,
    LC_LOAD_DYLINKER = 0xe,
    LC_UUID = 0x1b,
    LC_MAIN = (0x28 | LC_REQ_DYLD),
};


enum SymbolType : uint8_t
{
    N_GSYM = 0x20u,
    N_FNAME = 0x22u,
    N_FUN = 0x24u,

    N_STAB = 0xe0u,
    N_PEXT = 0x10u,
    N_TYPE = 0x0eu,
    N_EXT = 0x01u,



    N_UNDF = 0x0u,		/* undefined, n_sect == NO_SECT */
    N_ABS =	0x2u,		    /* absolute, n_sect == NO_SECT */
    N_SECT = 0xeu,		/* defined in section number n_sect */
    N_PBUD = 0xcu,		/* prebound undefined (defined in a dylib) */
    N_INDR = 0xau,		/* indirect */
};


/*
 * For the two types of symbol pointers sections and the symbol stubs section
 * they have indirect symbol table entries.  For each of the entries in the
 * section the indirect symbol table entries, in corresponding order in the
 * indirect symbol table, start at the index stored in the reserved1 field
 * of the section structure.  Since the indirect symbol table entries
 * correspond to the entries in the section the number of indirect symbol table
 * entries is inferred from the size of the section divided by the size of the
 * entries in the section.  For symbol pointers sections the size of the entries
 * in the section is 4 bytes and for symbol stubs sections the byte size of the
 * stubs is stored in the reserved2 field of the section structure.
 */
#define	S_NON_LAZY_SYMBOL_POINTERS	0x6	/* section with only non-lazy
						   symbol pointers */
#define	S_LAZY_SYMBOL_POINTERS		0x7	/* section with only lazy symbol
						   pointers */
#define	S_SYMBOL_STUBS			0x8	/* section with only symbol
						   stubs, byte size of stub in
						   the reserved2 field */
#define	S_MOD_INIT_FUNC_POINTERS	0x9	/* section with only function
						   pointers for initialization*/
#define	S_MOD_TERM_FUNC_POINTERS	0xa	/* section with only function
						   pointers for termination */
#define	S_COALESCED			0xb	/* section contains symbols that
						   are to be coalesced */
#define	S_GB_ZEROFILL			0xc	/* zero fill on demand section
						   (that can be larger than 4
						   gigabytes) */
#define	S_INTERPOSING			0xd	/* section with only pairs of
						   function pointers for
						   interposing */
#define	S_16BYTE_LITERALS		0xe	/* section with only 16 byte
						   literals */
#define	S_DTRACE_DOF			0xf	/* section contains
						   DTrace Object Format */
#define	S_LAZY_DYLIB_SYMBOL_POINTERS	0x10	/* section with only lazy
						   symbol pointers to lazy
						   loaded dylibs */

#define RELOCATION_TYPE_X86_64_UNSIGNED     0x0
#define RELOCATION_TYPE_X86_64_SIGNED       0x1
#define RELOCATION_TYPE_X86_64_BRANCH       0x2
#define RELOCATION_TYPE_X86_64_GOT_LOAD     0x3
#define RELOCATION_TYPE_X86_64_GOT          0x4
#define RELOCATION_TYPE_X86_64_SUBTRACTOR   0x5
#define RELOCATION_TYPE_X86_64_SIGNED1      0x6
#define RELOCATION_TYPE_X86_64_SIGNED2      0x7
#define RELOCATION_TYPE_X86_64_SIGNED4      0x8

#define VM_PROTECTION_NONE    0x0
#define VM_PROTECTION_READ    0x1
#define VM_PROTECTION_WRITE   0x2
#define VM_PROTECTION_EXECUTE 0x4
#define VM_PROTECTION_ALL (VM_PROTECTION_READ | VM_PROTECTION_WRITE | VM_PROTECTION_EXECUTE)


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


struct SymtabCommand
{
    uint32_t
    cmd;

    uint32_t
    cmdSize;

    uint32_t
    symbolOffset;

    uint32_t
    symbolEntries;

    uint32_t
    stringOffset;

    uint32_t
    stringSize;
};


struct DysymTabCommand
{
    uint32_t
    cmd;

    uint32_t
    cmdSize;

    uint32_t
    localSymbolIndex;

    uint32_t
    localSymbolEntries;

    uint32_t
    definedExternalSymbolIndex;

    uint32_t
    definedExternalSymbolEntries;

    uint32_t
    undefinedSymbolIndex;

    uint32_t
    undefinedSymbolNumber;

    uint32_t
    tableOfContentOffset;

    uint32_t
    tableOfContentEntries;

    uint32_t
    moduleTableOffset;

    uint32_t
    moduleTableEntries;

    uint32_t
    externalReferenceTableOffset;

    uint32_t
    externalReferenceTableEntries;

    uint32_t
    indirectSymbolTableOffset;

    uint32_t
    indirectSymbolTableEntries;

    uint32_t
    externalRelocationTableOffset;

    uint32_t
    externalRelocationTableEntries;

    uint32_t
    localRelocationTableOffset;

    uint32_t
    localRelocationTableEntries;
};


struct SymbolTableEntry
{
    uint32_t
    stringTableIndex;

    uint8_t
    sectionIndex;

    uint8_t
    description; // Use for STAB info

    uint64_t
    value;
};


struct Command
{
    uint32_t
    cmd;

    // Note cmdSize most be 4-byte-aligned
    uint32_t
    cmdSize;
};


struct LoadDylibCommand : Command
{
    uint32_t
    name;

    uint32_t
    timestamp;

    // 1 byte: patch
    // 2 byte: minor
    // 3-4 byte: major
    uint32_t
    currentVersion;

    // 1 byte: patch
    // 2 byte: minor
    // 3-4 byte: major
    uint32_t
    compatabilityVersion;
};


struct LoadDylinkerCommand : Command
{
    uint32_t
    name; // dynamic linker's path name
};


struct UuidCommand : Command
{
    uint8_t
    uuid[16] = {
        0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u,
    };
};


struct MainCommand : Command
{
    uint64_t
    offset;

    uint64_t
    stackSize;
};


#define VM_PROTECTION_NONE    0x1
#define VM_PROTECTION_READ    0x1
#define VM_PROTECTION_WRITE   0x2
#define VM_PROTECTION_EXECUTE 0x4
#define VM_PROTECTION_ALL (VM_PROTECTION_READ | VM_PROTECTION_WRITE | VM_PROTECTION_EXECUTE)


struct SegmentCommand64
{
    uint32_t
    command;

    // includes sizeof section_64 structs
    uint32_t
    commandSize;

    char
    segmentName[16];

    uint64_t
    vmAddress;

    uint64_t
    vmSize;

    uint64_t
    fileOffset;

    uint64_t
    fileSize;

    int
    maximumProtection;

    int
    initialProtection;

    uint32_t
    numberOfSections;

    uint32_t
    flags;
};



struct Section64
{
    char
    sectionName[16];

    char
    segmentName[16];

    uint64_t
    address;

    uint64_t
    size;

    uint32_t
    offset;

    uint32_t
    align;

    uint32_t
    relocationOffset;

    uint32_t
    numberOfRelocations;

    uint32_t
    flags;

    uint32_t
    reserved1;

    uint32_t
    reserved2;

    uint32_t
    reserved3;
};


class BaselineObjectFileWriter : public ObjectFileWriterInterface
{

public:

    BaselineObjectFileWriter();

    List<uint8_t>*
    write(FunctionRoutine* startRoutine) override;

    std::uint64_t
    vmAddress = 0x0000000100000000;

    std::uint64_t
    offset;

    DyldInfoCommand*
    dyldInfoCommand;

    DysymTabCommand*
    dysymTabCommand;

    SymtabCommand*
    symtabCommand;

    AssemblyWriterInterface*
    assemblyWriter;

    SegmentCommand64*
    linkEditSegment;

    uint64_t
    textSegmentStartOffset;

private:

    uint64_t
    _undefinedExternalSymbolIndex = 0;

    List<std::uint8_t>
    _text;

    DyldInfoWriter*
    _dyldInfoWriter;

    ByteWriter*
    _bw;

    MachHeader64*
    _header;

    std::uint32_t
    _textOffset;

    std::uint32_t
    _textSize;

    std::uint32_t
    _textSegmentSize;

    std::uint32_t
    _modTextSegmentSize;

    std::uint32_t
    _numberOfTextSections;

    uint64_t
    _textSegmentStartVmAddress;

    SegmentCommand64*
    _textSegment;

    SegmentCommand64*
    _dataConstSegment;

    SegmentCommand64*
    _dataSegment;

    Section64*
    _dataLaSymbolPtrSection;

    Section64*
    _dataSection;

    Section64*
    _dataConstGotSection;

    Section64*
    _textSection;

    Section64*
    _stubsSection;

    Section64*
    _stubHelperSection;

    Section64*
    _cstringSection;

    uint32_t
    _stringOffset;

    uint32_t
    _stringSize;

    uint32_t
    _stubsOffset;

    uint32_t
    _stubsSize;

    uint32_t
    _stubHelperOffset;

    uint64_t
    _dyldPrivateVmAddress;

    uint64_t
    _dyldPrivateStringTableIndexOffset;

    uint64_t
    _stubHelperSize;

    ExternalRoutine*
    _dyldStubBinderRoutine;

    MainCommand*
    _mainCommand;

    void
    writeHeader();

    void
    layoutTextSegment(FunctionRoutine* startRoutine);

    SegmentCommand64*
    writeSegment(SegmentCommand64 segmentCommand);

    void
    writePageZeroSegmentCommand();

    Section64*
    writeSection(Section64 section, SegmentCommand64* segment);

    void
    writePadding(uint32_t pad);

    void
    writeTextSegmentCommand();

    void
    writeDyldInfoSegmentCommand();

    void
    writeDyldInfo();

    void
    writeTextSegment();

    void
    writeLinkEditSegmentCommand();

    void
    writeDataConstSegmentCommand();

    void
    writeDataConstSegment();

    void
    writeStubHelperSection();

    void
    writeStubsSection();

    void
    writeTextSection(FunctionRoutine* startRoutine);

    void
    writeDataSegmentCommand();

    void
    writeDataSegment();

    void
    writeLaSymbolPtrSection();

    void
    writeDysymTabCommand();

    void
    writeSymtabCommand();

    void
    writeSymbolTable();

    void
    writeIndirectSymbolTable();

    void
    writeLoadDylibCommands();

    template<typename TCommand>
    TCommand*
    writeCommand(CommandType commandType);

    void
    writeStringTable();

    void
    writeLoadDylinkerCommand();

    void
    writeUuidCommand();

    template<typename TCommand>
    void
    writeCommandPadding(TCommand* command);

    void
    writeMainCommand(FunctionRoutine* startRoutine);

    void
    writeDataSection();

    void
    writeDyldPrivateSymbol();

    void
    writeCStringSection();
};


}


#endif //ELET_BASELINEOBJECTFILEWRITER_H
