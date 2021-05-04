#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Compiler/Syntax/Instruction.h>

namespace elet::domain::compiler
{
    struct Symbol;
}
namespace elet::domain::compiler::instruction
{
    struct Routine;
}

namespace elet::domain::compiler::instruction::output
{


class AssemblyWriterInterface
{

public:

    virtual
    void
    writeRoutine(Routine *routine) = 0;
};


}

#endif //ELET_ASSEMBLYWRITERINTERFACE_H
