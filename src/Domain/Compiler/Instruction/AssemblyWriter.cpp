#include "AssemblyWriter.h"
#include "Domain/Compiler/Instruction/Assembly/Baseline/BaselineWriter.h"
#include "Domain/Compiler/Instruction/Assembly/x86/X86_64Writer.h"

namespace elet::domain::compiler::instruction::output
{


thread_local
std::uint64_t
AssemblyWriter::_symbolOffset = 0;


AssemblyWriter::AssemblyWriter(compiler::AssemblyTarget target) :
    _target(target)
{
    _assemblyWriter = new X86_64Writer(nullptr);
}


void
AssemblyWriter::writeStartRoutine(FunctionRoutine* routine)
{
    _assemblyWriter->writeTextSection(routine);
}


}
