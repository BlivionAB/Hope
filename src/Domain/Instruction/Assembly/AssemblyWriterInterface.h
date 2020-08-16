#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Instruction/Instruction.h>

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

    explicit AssemblyWriterInterface(std::map<Utf8StringView, Symbol*>& symbolMap):
        _symbolMap(symbolMap)
    { }

    virtual
    void
    writeRoutine(Routine *routine) = 0;

protected:

    std::map<Utf8StringView, Symbol*>&
    _symbolMap;
};


}

#endif //ELET_ASSEMBLYWRITERINTERFACE_H
