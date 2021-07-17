#include "AssemblyWriter.h"
#include "Domain/Compiler/Instruction/Assembly/Baseline/BaselineWriter.h"
#include "Domain/Compiler/Instruction/Assembly/x86/X86_64Writer.h"
#include "Domain/Compiler/Instruction/Assembly/Aarch/Aarch64Writer.h"

namespace elet::domain::compiler::instruction::output
{


thread_local
std::uint64_t
AssemblyWriter::_symbolOffset = 0;


AssemblyWriter::AssemblyWriter(compiler::AssemblyTarget target) :
    _target(target)
{
    switch (target)
    {
        case AssemblyTarget::x86_64:
            _assemblyWriter = new X86_64Writer(nullptr);
            break;
        case AssemblyTarget::AArch64:
            _assemblyWriter = new Aarch64Writer(nullptr);
            break;
        default:
            throw std::runtime_error("Unknown assembly target.");
    }
}


void
AssemblyWriter::writeStartRoutine(FunctionRoutine* routine)
{
    _assemblyWriter->writeTextSection(routine);
}


}
