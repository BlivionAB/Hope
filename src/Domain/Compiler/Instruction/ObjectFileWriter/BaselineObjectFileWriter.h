#ifndef ELET_BASELINEOBJECTFILEWRITER_H
#define ELET_BASELINEOBJECTFILEWRITER_H

#include "../AssemblyWriter.h"
#include "../Assembly/x86/X86Parser/X86BaselinePrinter.h"


namespace elet::domain::compiler::instruction::output
{


class BaselineObjectFileWriter : public ObjectFileWriterInterface
{

public:

    BaselineObjectFileWriter();

    void
    write(FunctionRoutine* startRoutine) override;

private:

    void
    writeTextSection(FunctionRoutine* startRoutine);

    List<std::uint8_t>
    _output;

    std::size_t
    _currentOffset = 0;

    AssemblyWriterInterface*
    _assemblyWriter;

    x86::X86BaselinePrinter*
    _baselinePrinter;

    void
    writeHeader();
};


}


#endif //ELET_BASELINEOBJECTFILEWRITER_H
