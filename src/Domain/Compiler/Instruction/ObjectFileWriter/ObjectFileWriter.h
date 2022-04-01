#ifndef ELET_OBJECTFILEWRITER_H
#define ELET_OBJECTFILEWRITER_H

#include <map>
#include <Domain/Compiler/Instruction/Instruction.h>
#include <Domain/Compiler/Instruction/Assembly/Aarch/Aarch64Writer.h>
#include <Domain/Compiler/Instruction/Assembly/X86/X86_64Writer.h>
#include <Domain/Compiler/CompilerTypes.h>


namespace elet::domain::compiler::instruction::output
{
    struct RelocationOperand;


    struct AssemblySegments
    {
        std::uint64_t
        textSize;

        List<Utf8StringView*>*
        cstrings;

        std::size_t
        cstringSize;

        List<ast::Symbol*>*
        symbols;

        std::size_t
        symbolSize;

        List<RelocationOperand*>*
        symbolicRelocations;

        List<RelocationOperand*>*
        relativeRelocations;
    };


    class ObjectFileWriter
    {

    public:

        ObjectFileWriter(AssemblyTarget assemblyTarget):
            _assemblyTarget(assemblyTarget)
        {
            _bw = new ByteWriter(&output, &offset);
            switch (assemblyTarget)
            {
                case AssemblyTarget::x86_64:
                    assemblyWriter = new x86::X86_64Writer(&_text);
                    break;
                case AssemblyTarget::Aarch64:
                    assemblyWriter = new Aarch64Writer(&_text);
                    break;
            }
        }

        virtual
        void
        beginWrite()
        {

        }


        virtual
        void
        write(FunctionRoutine* function) = 0;


        virtual
        void
        endWrite()
        {

        }

        List<uint8_t>&
        getOutput()
        {
            return output;
        }

        List<uint8_t>
        output;

        AssemblyWriterInterface*
        assemblyWriter;

        uint64_t
        offset = 0;

    protected:

        ByteWriter*
        _bw;

        List<uint8_t>
        _text;

        compiler::AssemblyTarget
        _assemblyTarget;
    };
}


#endif //ELET_OBJECTFILEWRITER_H
