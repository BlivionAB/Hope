#ifndef ELET_DYLDINFOWRITER_H
#define ELET_DYLDINFOWRITER_H


#include <cstdint>
#include <Foundation/List.h>
#include "MachoFileWriter.h"
#include "ExportInfoWriter.h"


namespace elet::domain::compiler::instruction::output::macho
{
    using namespace elet::foundation;

    class MachoFileWriter;
    class ExportInfoWriter;

    class DyldInfoWriter
    {
    public:

        DyldInfoWriter(MachoFileWriter* _machoWriter);

        void
        write();

    private:

        ByteWriter*
        _bw;

        MachoFileWriter*
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
