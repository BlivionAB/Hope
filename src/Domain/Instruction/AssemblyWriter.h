#ifndef ELET_ASSEMBLYWRITER_H
#define ELET_ASSEMBLYWRITER_H


#include "Domain/Instruction/Assembly/x86/X86_64Writer.h"
#include "Domain/Compiler.h"


namespace elet::domain::compiler
{
    enum class AssemblyTarget;
}


namespace elet::domain::compiler::instruction::output
{


class AssemblyWriter
{

public:

    AssemblyWriter(AssemblyTarget target, std::map<Routine*, List<std::uint8_t>*>& output, std::mutex& workMutex);

    List<std::uint8_t>*
    writeRoutine(Routine *routine);

private:

    AssemblyTarget
    _target;

    AssemblyWriterInterface*
    _assemblyWriter;
};


}


#endif //ELET_ASSEMBLYWRITER_H
