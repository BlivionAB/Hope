#ifndef ELET_DYLDINFOWRITER_H
#define ELET_DYLDINFOWRITER_H


#include <cstdint>
#include <Foundation/List.h>
#include "BaselineObjectFileWriter.h"
#include "ExportInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


using namespace elet::foundation;

class BaselineObjectFileWriter;
class ExportInfoWriter;

class DyldInfoWriter
{
public:

    DyldInfoWriter(BaselineObjectFileWriter* _machoWriter);

    void
    write();

private:

    ByteWriter*
    _bw;

    BaselineObjectFileWriter*
    _machoWriter;

    ExportInfoWriter*
    _exportInfoWriter;

    void
    writeRebaseInfo();

    void
    writeBindingInfo();

    void
    writeLazyBindingInfo();

    void
    writeExportInfo();
};


}
#endif //ELET_DYLDINFOWRITER_H
