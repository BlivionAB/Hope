#include "AssemblyWriter.h"
#include "Assembly/Baseline/BaselineWriter.h"
#include "Assembly/x86/X86_64Writer.h"

namespace elet::domain::compiler::instruction::output
{


AssemblyWriter::AssemblyWriter(compiler::AssemblyTarget target, std::map<Routine*, List<std::uint8_t>*>& output, std::mutex& workMutex) :
    _target(target)
{
    switch (_target)
    {
        case compiler::AssemblyTarget::Baseline:
            _assemblyWriter = new BaselineWriter(output, workMutex);
            break;
        case compiler::AssemblyTarget::x86_64:
            _assemblyWriter = new X86_64Writer(output, workMutex);
            break;
    }
}

List<std::uint8_t>*
AssemblyWriter::writeRoutine(Routine *routine)
{
    return _assemblyWriter->writeRoutine(routine);
}


}
