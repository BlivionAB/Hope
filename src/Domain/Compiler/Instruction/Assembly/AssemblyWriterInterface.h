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

    AssemblyWriterInterface(List<std::uint8_t>* output):
        _output(output)
    { }

    virtual
    void
    writeStartRoutine(FunctionRoutine* routine, std::size_t offset) = 0;

    List<std::uint8_t>*
    getOutput()
    {
        return _output;
    }

protected:

    List<std::uint8_t>*
    _output;
};


}

#endif //ELET_ASSEMBLYWRITERINTERFACE_H
