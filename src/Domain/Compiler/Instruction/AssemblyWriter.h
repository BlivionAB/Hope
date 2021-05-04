#ifndef ELET_ASSEMBLYWRITER_H
#define ELET_ASSEMBLYWRITER_H


#include "Domain/Compiler/Instruction/Assembly/x86/X86_64Writer.h"
#include "Domain/Compiler/Compiler.h"


namespace elet::domain::compiler
{
    enum class AssemblyTarget;
}


namespace elet::domain::compiler::instruction::output
{


struct AssemblyWriterInterface;


class AssemblyWriter
{

public:

    AssemblyWriter(AssemblyTarget target);

private:

    AssemblyTarget
    _target;

    static
    thread_local
    std::uint64_t
    _symbolOffset;

    AssemblyWriterInterface*
    _assemblyWriter;
};


}


#endif //ELET_ASSEMBLYWRITER_H
