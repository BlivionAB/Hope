#ifndef ELET_BASELINEOBJECTFILEWRITER_H
#define ELET_BASELINEOBJECTFILEWRITER_H

#include "../AssemblyWriter.h"
#include "MachoFileWriter.h"
#include "DyldInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


class DyldInfoWriter;
// For reference: https://opensource.apple.com/source/xnu/xnu-4903.270.47/EXTERNAL_HEADERS/mach-o/loader.h.auto.html
#define SEGMENT_SIZE 0x4000 // 8 byte alignemnt

enum MachoMagicValue : uint64_t
{
    MACHO_MAGIC_64 = 0xfeedfacf,
};


enum MachoCpuType
{
    CPU_ARCH_ABI64 = 0x01000000,
    CPU_TYPE_X86 = 0x7,
    CPU_TYPE_X86_64 = (CPU_TYPE_X86 | CPU_ARCH_ABI64),
    CPU_SUBTYPE_X86_64_ALL = 0x3,
    CPU_TYPE_ARM = 0xc,
    CPU_TYPE_ARM64 = (CPU_TYPE_ARM | CPU_ARCH_ABI64),
    CPU_SUBTYPE_ARM64_ALL = 0x0,
};


enum VmProtection
{
    VM_PROTECTION_NONE = 0x0,
    VM_PROTECTION_READ = 0x1,
    VM_PROTECTION_WRITE = 0x2,
    VM_PROTECTION_EXECUTE = 0x4,
    VM_PROTECTION_ALL = (VM_PROTECTION_READ | VM_PROTECTION_WRITE | VM_PROTECTION_EXECUTE),
};


enum MachoFileType
{
    MH_EXECUTE = 0x2,
};


enum MachoFlags
{
    MH_NOUNDEFS = 0x00000001,
    MH_DYLDLINK = 0x00000004,
    MH_TWOLEVEL = 0x00000080,
    MH_PIE = 0x00200000,
};


enum MachoSectionFlags
{
    S_ATTR_PURE_INSTRUCTIONS = 0x80000000,
    S_ATTR_SOME_INSTRUCTIONS = 0x00000400,

    S_CSTRING_LITERALS = 0x2,
    S_SYMBOL_STUBS = 0x8,
    S_NON_LAZY_SYMBOL_POINTERS = 0x6,
    S_LAZY_SYMBOL_POINTERS = 0x7,
};


enum RebaseOpcode
{
    REBASE_TYPE_POINTER = 0x1,
    REBASE_OPCODE_DONE = 0x00,
    REBASE_OPCODE_SET_TYPE_IMM = 0x10,
    REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB = 0x20,
    REBASE_OPCODE_DO_REBASE_IMM_TIMES = 0x50,
};


enum BindOpcode
{
    BIND_TOP_POINTER = 0x1,
    BIND_OPCODE_DONE = 0x00,
    BIND_OPCODE_SET_DYLIB_ORDINAL_IMM = 0x10,
    BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM = 0x40,
    BIND_OPCODE_SET_TYPE_IMM = 0x50,
    BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB = 0x70,
    BIND_OPCODE_DO_BIND = 0x90,
};


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


struct MachHeader64
{
    uint32_t
    magic;

    uint32_t
    cpuType;

    uint32_t
    cpuSubType;

    uint32_t
    fileType;

    uint32_t
    numberOfCommands;

    uint32_t
    sizeOfCommands;

    uint32_t
    flags;

    uint32_t
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


class MachoFileWriter : public ObjectFileWriterInterface
{

public:

    MachoFileWriter(AssemblyTarget assemblyTarget);

    List<uint8_t>*
    write(FunctionRoutine* startRoutine) override;

    uint64_t
    vmAddress = 0x0000000100000000;

    uint64_t
    offset = 0;

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

    List<uint8_t>
    _text;

    DyldInfoWriter*
    _dyldInfoWriter;

    ByteWriter*
    _bw;

    MachHeader64*
    _header;

    AssemblyTarget
    _assemblyTarget;

    uint32_t
    _textOffset;

    uint32_t
    _textSize;

    uint32_t
    _textSegmentSize;

    uint32_t
    _modTextSegmentSize;

    uint32_t
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

    void
    relocateCStrings(uint64_t offset);
};


}


#endif //ELET_BASELINEOBJECTFILEWRITER_H
