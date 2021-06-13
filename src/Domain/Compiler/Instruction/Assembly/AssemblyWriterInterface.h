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

    List<std::uint8_t>*
    getOutput();

    std::size_t
    getOffset();

    std::uint32_t
    getExternRoutinesSize() const;

    ByteWriter*
    bw;

    List<FunctionRoutine*>
    internalRoutines;

    List<ExternalRoutine*>
    externalRoutines;

    List<ExternalRoutine*>
    gotBoundRoutines;

    List<FunctionRoutine*>
    exportedRoutines;

    void
    writePadding(size_t amount);

protected:

    List<std::uint8_t>*
    _output;

    List<String*>
    _strings;

    std::size_t
    _offset;

    std::size_t
    _dataDataRelocationAddress;

    std::size_t
    _dyldStubBinderRelocationAddress;
};

}


#endif //ELET_ASSEMBLYWRITERINTERFACE_H
