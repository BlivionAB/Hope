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


struct String;


class AssemblyWriterInterface
{

public:

    AssemblyWriterInterface(List<std::uint8_t>* output);

    virtual
    void
    writeTextSection(FunctionRoutine* routine) = 0;

    void
    writeCStringSection();

    virtual
    void
    writeStubs() = 0;

    virtual
    void
    writeStubHelper() = 0;

    std::size_t
    __dataDataRelocationAddress;

    bool
    hasStrings();

    List<std::uint8_t>*
    getOutput();

    std::size_t
    getOffset();

    void
    writeByte(std::uint8_t instruction);

    bool
    hasExternalRoutines();

protected:

    void
    writeQuadWord(std::uint64_t instruction);


    void
    writeDoubleWord(std::uint32_t instruction);


    void
    writeDoubleWordAtAddress(std::uint32_t instruction, std::size_t offset);


    List<std::uint8_t>*
    _output;

    List<String*>
    _strings;

    List<ExternalRoutine*>
    _externalRoutines;

    std::size_t
    _currentOffset;
};

}


#endif //ELET_ASSEMBLYWRITERINTERFACE_H
