#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Compiler/Instruction/Instruction.h>

namespace elet::domain::compiler
{
    struct Symbol;
}
namespace elet::domain::compiler::instruction
{
    struct FunctionRoutine;
}

namespace elet::domain::compiler::instruction::output
{


class AssemblyWriterInterface
{

public:

    virtual
    void
    writeStartRoutine(FunctionRoutine* routine, std::size_t offset) = 0;
};


}

#endif //ELET_ASSEMBLYWRITERINTERFACE_H
