#ifndef ELET_OBJECTFILEWRITER_H
#define ELET_OBJECTFILEWRITER_H

#include <Foundation/FilePath.h>
#include <Domain/Compiler/Instruction/ObjectFileWriter/ObjectFileWriterInterface.h>

namespace elet::domain::compiler::instruction::output
{


struct AssemblySegments;
struct ObjectFileWriterInterface;


using namespace elet::foundation;

enum class ObjectFileTarget
{
    Unknown,
    MachO,
    Elf,
    Coff,
};


}

#endif //ELET_OBJECTFILEWRITER_H
