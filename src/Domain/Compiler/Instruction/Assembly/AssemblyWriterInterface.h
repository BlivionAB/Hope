#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Compiler/Instruction/Instruction.h>
#include <Domain/Compiler/Instruction/ByteWriter.h>

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

    bool
    hasStrings();

    List<std::uint8_t>*
    getOutput();

    std::size_t
    getOffset();

    bool
    hasExternalRoutines();

    std::uint32_t
    getExternRoutinesSize() const;

protected:

    List<std::uint8_t>*
    _output;

    List<String*>
    _strings;

    List<ExternalRoutine*>
    _externalRoutines;

    std::size_t
    _currentOffset;

    ByteWriter*
    _bw;

    std::size_t
    _dataDataRelocationAddress;

    std::size_t
    _dyldStubBinderRelocationAddress;

    List<std::size_t>
    _dyldStubOffsetRelocationAddress;
};

}


#endif //ELET_ASSEMBLYWRITERINTERFACE_H
