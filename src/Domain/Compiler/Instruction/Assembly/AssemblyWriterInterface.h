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



struct CallingConvention
{
    List<uint8_t>
    registers;
};


class AssemblyWriterInterface
{

public:

    AssemblyWriterInterface(List<uint8_t>* output);

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

    List<uint8_t>*
    getOutput();

    uint64_t
    getOffset();

    uint32_t
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

    uint64_t
    dyldPrivateOffset;

protected:

    CallingConvention
    _callingConvention;

    List<uint8_t>*
    _output;

    List<String*>
    _strings;

    uint64_t
    _offset = 0;

    uint64_t
    _dyldStubBinderRelocationAddress;

    uint64_t
    getStackSizeFromFunctionParameters(const FunctionRoutine* routine);
};

}


#endif //ELET_ASSEMBLYWRITERINTERFACE_H
