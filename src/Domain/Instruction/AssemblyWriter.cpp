#include "AssemblyWriter.h"
#include "Assembly/Baseline/BaselineWriter.h"
#include "Assembly/x86/X86_64Writer.h"

namespace elet::domain::compiler::instruction::output
{


thread_local
std::uint64_t
AssemblyWriter::_symbolOffset = 0;


AssemblyWriter::AssemblyWriter(compiler::AssemblyTarget target) :
    _target(target)
{
    switch (_target)
    {
        case AssemblyTarget::Baseline:
            _assemblyWriter = new BaselineWriter();
            break;
        case AssemblyTarget::x86_64:
            _assemblyWriter = new X86_64Writer();
            break;
    }
}


void
AssemblyWriter::writeRoutine(Routine *routine)
{
    routine->symbolicRelocations = new List<RelocationOperand*>();
    routine->relativeRelocations = new List<RelocationOperand*>();
//    routine->symbol->textOffset = _symbolOffset;
    _assemblyWriter->writeRoutine(routine);
    _symbolOffset += routine->machineInstructions->size();
}


}
