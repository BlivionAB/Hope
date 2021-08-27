#ifndef ELET_OBJECTFILEWRITER_H
#define ELET_OBJECTFILEWRITER_H


#include <Domain/Compiler/Instruction/ObjectFileWriter/ObjectFileWriterInterface.h>

namespace elet::domain::compiler::instruction::output
{


struct AssemblySegments;


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
