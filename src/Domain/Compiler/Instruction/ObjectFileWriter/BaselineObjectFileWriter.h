#ifndef ELET_BASELINEOBJECTFILEWRITER_H
#define ELET_BASELINEOBJECTFILEWRITER_H

#include "../AssemblyWriter.h"
#include "../Assembly/x86/X86Parser/X86BaselinePrinter.h"
#include "MachOFileWriter.h"


namespace elet::domain::compiler::instruction::output
{


// For reference: https://opensource.apple.com/source/xnu/xnu-4903.270.47/EXTERNAL_HEADERS/mach-o/loader.h.auto.html

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

#define VM_PROTECTION_NONE    0x0
#define VM_PROTECTION_READ    0x1
#define VM_PROTECTION_WRITE   0x2
#define VM_PROTECTION_EXECUTE 0x4
#define VM_PROTECTION_ALL (VM_PROTECTION_READ | VM_PROTECTION_WRITE | VM_PROTECTION_EXECUTE)


//struct MachHeader64
//{
//    std::uint32_t
//    magic = MACHO_MAGIC_64;
//
//    std::uint32_t
//    cpuType = CPU_TYPE_X86_64;
//
//    std::uint32_t
//    cpuSubType = CPU_SUBTYPE_X86_64_ALL;
//
//    std::uint32_t
//    fileType;
//
//    std::uint32_t
//    numberOfCommands = 0;
//
//    std::uint32_t
//    sizeOfCommands = 0;
//
//    std::uint32_t
//    flags;
//
//    std::uint32_t
//    reserved;
//};


//struct SegmentCommand64
//{
//    std::uint32_t
//    command;
//
//    // includes sizeof section_64 structs
//    std::uint32_t
//    commandSize;
//
//    char
//    segmentName[16];
//
//    std::uint64_t
//    vmAddress;
//
//    std::uint64_t
//    virtualMemorySize;
//
//    std::uint64_t
//    fileOffset;
//
//    std::uint64_t
//    fileSize;
//
//    int
//    maximumProtection;
//
//    int
//    initialProtection;
//
//    std::uint32_t
//    numberOfSections;
//
//    std::uint32_t
//    flags;
//};


//struct Section64
//{
//    std::array<char, 16>
//    sectionName;
//
//    std::array<char, 16>
//    segmentName;
//
//    std::uint64_t
//    address;
//
//    std::uint64_t
//    size;
//
//    std::uint32_t
//    offset;
//
//    std::uint32_t
//    align;
//
//    std::uint32_t
//    relocationOffset;
//
//    std::uint32_t
//    numberOfRelocations;
//
//    std::uint32_t
//    flags;
//
//    std::uint32_t
//    reserved1;
//
//    std::uint32_t
//    reserved2;
//
//    std::uint32_t
//    reserved3;
//};


class BaselineObjectFileWriter : public ObjectFileWriterInterface
{

public:

    BaselineObjectFileWriter();

    void
    write(FunctionRoutine* startRoutine) override;

private:

    List<std::uint8_t>
    _output;

    std::size_t
    _cursor;

    List<std::uint8_t>
    _text;

    std::size_t
    _currentOffset = 0;

    AssemblyWriterInterface*
    _assemblyWriter;

    x86::X86BaselinePrinter*
    _baselinePrinter;

    MachHeader64*
    _header = new MachHeader64 { MACHO_MAGIC_64, CPU_TYPE_X86_64, CPU_SUBTYPE_X86_64_ALL, MH_EXECUTE };

    std::size_t
    _textOffset;

    std::uint32_t
    _textSize;

    std::uint32_t
    _numberOfTextSections;

    std::size_t
    _stringOffset;

    std::size_t
    _stubOffset;

    void
    writeHeader();

    void
    layoutTextSegment(FunctionRoutine* startRoutine);

    SegmentCommand64*
    writeSegment(SegmentCommand64 segmentCommand);

    void
    writePageZeroSegmentCommand();

    Section64*
    writeSection(Section64 section);

    void
    writePadding(uint32_t pad);

    void
    writeTextSegmentCommand();
};


}


#endif //ELET_BASELINEOBJECTFILEWRITER_H
