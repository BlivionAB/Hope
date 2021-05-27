#ifndef ELET_DYLDINFOWRITER_H
#define ELET_DYLDINFOWRITER_H


#include <cstdint>
#include <Foundation/List.h>
#include "BaselineObjectFileWriter.h"

namespace elet::domain::compiler::instruction::output
{


using namespace elet::foundation;


class DyldInfoWriter
{
public:

    DyldInfoWriter(BaselineObjectFileWriter* _machoWriter);

    void
    write();

private:

    BaselineObjectFileWriter*
    _machoWriter;

    List<uint8_t>
    _data;

    size_t
    _offset;

    ByteWriter*
    _bw;

    void
    writeRebaseInfo();

    void
    writeBindingInfo();

    void
    writeLazyBindingInfo();
};


}
#endif //ELET_DYLDINFOWRITER_H
