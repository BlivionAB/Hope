#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Instruction/Instruction.h>


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

    AssemblyWriterInterface(std::map<output::Routine*, List<std::uint8_t>*>& output, std::mutex& workMutex):
        _output(output)
    { }

    virtual
    List<std::uint8_t>*
    writeRoutine(Routine *routine) = 0;

protected:

    std::map<Routine*, AssemblySymbol>
    _symbolTable;

    Utf8String
    _stringTable;

    std::map<output::Routine*, List<std::uint8_t>*>&
    _output;
};


}

#endif //ELET_ASSEMBLYWRITERINTERFACE_H
