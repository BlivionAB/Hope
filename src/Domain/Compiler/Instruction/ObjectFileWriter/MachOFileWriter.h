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



/*
 * After MacOS X 10.1 when a new load command is added that is required to be
 * understood by the dynamic linker for the image to execute properly the
 * LC_REQ_DYLD bit will be or'ed into the load command constant.  If the dynamic
 * linker sees such a load command it it does not understand will issue a
 * "unknown load command required for execution" error and refuse to use the
 * image.  Other load commands without this bit that are not understood will
 * simply be ignored.
 */
#define LC_REQ_DYLD 0x80000000

/* Constants for the cmd field of all load commands, the type */
#define	LC_SEGMENT	0x1	/* segment of this file to be mapped */
#define	LC_SYMSEG	0x3	/* link-edit gdb symbol table info (obsolete) */
#define	LC_THREAD	0x4	/* thread */
#define	LC_UNIXTHREAD	0x5	/* unix thread (includes a stack) */
#define	LC_LOADFVMLIB	0x6	/* load a specified fixed VM shared library */
#define	LC_IDFVMLIB	0x7	/* fixed VM shared library identification */
#define	LC_IDENT	0x8	/* object identification info (obsolete) */
#define LC_FVMFILE	0x9	/* fixed VM file inclusion (internal use) */
#define LC_PREPAGE      0xa     /* prepage command (internal use) */
#define	LC_ID_DYLIB	0xd	/* dynamically linked shared lib ident */
#define LC_LOAD_DYLINKER 0xe	/* load a dynamic linker */
#define LC_ID_DYLINKER	0xf	/* dynamic linker identification */
#define	LC_PREBOUND_DYLIB 0x10	/* modules prebound for a dynamically */
/*  linked shared library */
#define	LC_ROUTINES	0x11	/* image routines */
#define	LC_SUB_FRAMEWORK 0x12	/* sub framework */
#define	LC_SUB_UMBRELLA 0x13	/* sub umbrella */
#define	LC_SUB_CLIENT	0x14	/* sub client */
#define	LC_SUB_LIBRARY  0x15	/* sub library */
#define	LC_TWOLEVEL_HINTS 0x16	/* two-level namespace lookup hints */
#define	LC_PREBIND_CKSUM  0x17	/* prebind checksum */

/*
 * load a dynamically linked shared library that is allowed to be missing
 * (all symbols are weak imported).
 */
#define	LC_LOAD_WEAK_DYLIB (0x18 | LC_REQ_DYLD)

#define	LC_SEGMENT_64	0x19	/* 64-bit segment of this file to be
				   mapped */
#define	LC_ROUTINES_64	0x1a	/* 64-bit image routines */
#define LC_UUID		0x1b	/* the uuid */
#define LC_RPATH       (0x1c | LC_REQ_DYLD)    /* runpath additions */
#define LC_CODE_SIGNATURE 0x1d	/* local of code signature */
#define LC_SEGMENT_SPLIT_INFO 0x1e /* local of info to split segments */
#define LC_REEXPORT_DYLIB (0x1f | LC_REQ_DYLD) /* load and re-export dylib */
#define	LC_LAZY_LOAD_DYLIB 0x20	/* delay load of dylib until first use */
#define	LC_ENCRYPTION_INFO 0x21	/* encrypted segment information */
#define	LC_DYLD_INFO 	0x22	/* compressed dyld information */
#define	LC_DYLD_INFO_ONLY (0x22|LC_REQ_DYLD)	/* compressed dyld information only */
#define	LC_LOAD_UPWARD_DYLIB (0x23 | LC_REQ_DYLD) /* load upward dylib */
#define LC_VERSION_MIN_MACOSX 0x24   /* build for MacOSX min OS version */
#define LC_VERSION_MIN_IPHONEOS 0x25 /* build for iPhoneOS min OS version */
#define LC_FUNCTION_STARTS 0x26 /* compressed table of function start addresses */
#define LC_DYLD_ENVIRONMENT 0x27 /* string for dyld to treat
				    like environment variable */
#define LC_MAIN (0x28|LC_REQ_DYLD) /* replacement for LC_UNIXTHREAD */
#define LC_DATA_IN_CODE 0x29 /* table of non-instructions in __text */
#define LC_SOURCE_VERSION 0x2A /* source version used to build binary */
#define LC_DYLIB_CODE_SIGN_DRS 0x2B /* Code signing DRs copied from linked dylibs */
#define	LC_ENCRYPTION_INFO_64 0x2C /* 64-bit encrypted segment information */
#define LC_LINKER_OPTION 0x2D /* linker options in MH_OBJECT files */
#define LC_LINKER_OPTIMIZATION_HINT 0x2E /* optimization hints in MH_OBJECT files */
#define LC_VERSION_MIN_TVOS 0x2F /* build for AppleTV min OS version */
#define LC_VERSION_MIN_WATCHOS 0x30 /* build for Watch min OS version */
#define LC_NOTE 0x31 /* arbitrary data included within a Mach-O file */
#define LC_BUILD_VERSION 0x32 /* build for platform min OS version */


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


///*
// * The following are used on the flags byte of a terminal node
// * in the export information.
// */
//#define EXPORT_SYMBOL_FLAGS_KIND_MASK				0x03
//#define EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION			0x04
//#define EXPORT_SYMBOL_FLAGS_REEXPORT				0x08
//#define EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER		0x10


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
    uint32_t
    index;

    /* type flag, see below */
    uint8_t
    type;

    /* section number or NO_SECT */
    uint8_t
    sectionIndex;

    /* see <mach-o/stab.h> */
    uint16_t
    description;

    /* value of this symbol (or stab offset) */
    uint64_t
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
//#define	N_STAB	(uint8_t)0xe0  /* if any of these bits set, a symbolic debugging entry */
//#define	N_PEXT	(uint8_t)0x10  /* private external symbol bit */
//#define	N_SECT	(uint8_t)0x0e  /* mask for the type bits */
//#define	N_EXT	(uint8_t)0x01  /* external symbol bit, set for external symbols */

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


}

#endif //ELET_MACHOFILEWRITER_H
