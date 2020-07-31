#include "ObjectFileWriter.h"
#include "ObjectFileWriter/MachOFileWriter.h"

namespace elet::domain::compiler::instruction::output
{


ObjectFileWriter::ObjectFileWriter(ObjectFileTarget objectFileTarget)
{
    switch (objectFileTarget)
    {
        case ObjectFileTarget::MachO:
            _writer = new MachOFileWriter();
            break;
    }
}

void
ObjectFileWriter::writeToFile(const Path &path, const AssemblySegments& segments)
{
    _writer->writeToFile(path, segments);
}


}
