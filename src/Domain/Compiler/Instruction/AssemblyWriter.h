#ifndef ELET_ASSEMBLYWRITER_H
#define ELET_ASSEMBLYWRITER_H


#include "Domain/Compiler/Instruction/Assembly/x86/X86_64Writer.h"
#include "Domain/Compiler/Compiler.h"
#include "Instruction.h"


namespace elet::domain::compiler
{
    enum class AssemblyTarget;
}


namespace elet::domain::compiler::instruction::output
{
    struct FunctionRoutine;
    struct AssemblyWriterInterface;


    class AssemblyWriter
    {

    public:

        AssemblyWriter(compiler::AssemblyTarget target);

        void
        writeStartRoutine(FunctionRoutine* routine);

    private:

        compiler::AssemblyTarget
        _target;

        static
        thread_local
        std::uint64_t
        _symbolOffset;

        AssemblyWriterInterface*
        _assemblyWriter;
    };
}


#endif //ELET_ASSEMBLYWRITER_H
