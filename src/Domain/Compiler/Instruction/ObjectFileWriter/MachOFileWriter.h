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

#define CPU_TYPE_X86            0x7
#define CPU_TYPE_X86_64 (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_SUBTYPE_X86_64_ALL 0x3

#define S_ATTR_PURE_INSTRUCTIONS 0x80000000
#define S_ATTR_SOME_INSTRUCTIONS 0x00000400
#define S_CSTRING_LITERALS       0x00000002



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




enum class SectionDataType : std::uint8_t
{
    Assembly,
    CString,
    Constants,
};


}

#endif //ELET_MACHOFILEWRITER_H
