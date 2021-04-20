#ifndef ELET_OBJECTFILEWRITER_H
#define ELET_OBJECTFILEWRITER_H

#include <Foundation/Path.h>
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


class ObjectFileWriter
{

public:

    ObjectFileWriter(ObjectFileTarget objectFileTarget);

    void
    writeToFile(const Path& path, const AssemblySegments& segments);

private:

    ObjectFileWriterInterface*
    _writer;
};


}

#endif //ELET_OBJECTFILEWRITER_H
