#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Instruction/Instruction.h>

namespace elet::domain::compiler::instruction
{
    struct Routine;
}

namespace elet::domain::compiler::instruction::output
{


struct AssemblySymbol
{
    Utf8StringView
    name;

    Routine*
    routine;
};


class AssemblyWriterInterface
{

public:

    virtual
    void
    writeRoutine(Routine *routine) = 0;
};


}

#endif //ELET_ASSEMBLYWRITERINTERFACE_H
