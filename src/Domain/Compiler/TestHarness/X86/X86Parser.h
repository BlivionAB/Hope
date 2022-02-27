#ifndef ELET_X86PARSER_H
#define ELET_X86PARSER_H


#include <cstdint>
#include "X86Types.h"
#include "Domain/Compiler/TestHarness/AssemblyParser.h"
#include "Foundation/List.h"


namespace elet::domain::compiler::test::x86
{
    using namespace elet::foundation;


    class X86Parser : public AssemblyParser
    {

    public:

        X86Parser();

        void
        parse(List<Instruction>& instructions, List<std::uint8_t>& output, size_t offset, size_t size);

    private:

        uint8_t
        getByte(Instruction& instruction);

        std::array<uint8_t, 2>
        getWord(Instruction& instruction);

        std::array<uint8_t, 4>
        getDoubleWord(Instruction& instruction);

        Ev*
        createE(uint8_t modrmByte, Instruction& instruction, bool useOnlyRmField);

        Gv*
        createGv(uint8_t opcode, bool isQuadWord);

        Register
        mapDoubleWordRegisterIndex(uint8_t reg);

        Register
        mapQuadWordRegisterIndex(uint8_t reg);

        MemoryAddress32
        createMemoryAddress32(Instruction& instruction);

        void
        parseOneByteOpCode(List<Instruction>& instructions, Instruction& instruction, uint8_t opcode);

        void
        parseTwoByteOpCode(uint8_t opcode, Instruction& instruction);

        void
        parseThreeByteOpCode(uint8_t opcode, Instruction& instruction, List <Instruction>& instructions);
    };
}


#endif //ELET_X86PARSER_H
