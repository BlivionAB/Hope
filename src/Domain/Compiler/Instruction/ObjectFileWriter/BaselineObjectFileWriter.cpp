#include "BaselineObjectFileWriter.h"

namespace elet::domain::compiler::instruction::output
{


BaselineObjectFileWriter::BaselineObjectFileWriter()
{
    _assemblyWriter = new X86_64Writer(&_output);
    _baselinePrinter = new x86::X86BaselinePrinter();
}


void
BaselineObjectFileWriter::write(FunctionRoutine* startRoutine)
{
    writeHeader();
    writeTextSection(startRoutine);
}


void
BaselineObjectFileWriter::writeTextSection(FunctionRoutine* startRoutine)
{
    _assemblyWriter->writeStartRoutine(startRoutine, _currentOffset);
    auto output = _assemblyWriter->getOutput();
    _baselinePrinter->print(output);

}

void
BaselineObjectFileWriter::writeHeader()
{

}


}
