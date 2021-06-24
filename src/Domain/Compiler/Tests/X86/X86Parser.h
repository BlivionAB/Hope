#ifndef ELET_X86PARSER_H
#define ELET_X86PARSER_H


#include <cstdint>
#include "X86Types.h"
#include "Foundation/List.h"


namespace elet::domain::compiler::test
{


using namespace elet::foundation;


class X86Parser
{

public:

    X86Parser();

    List<Instruction*>
    parse(List<std::uint8_t>& output, size_t offset, size_t size);

private:

    List<std::uint8_t>*
    _output;

    size_t
    _offset;

    size_t
    _size;

    size_t
    _cursor = 0;

    uint8_t
    getByte(Instruction* instruction);

    std::array<std::uint8_t, 4>
    getDoubleWord(Instruction* instruction);

    Ev*
    createEv(std::uint8_t modrmByte, Instruction* instruction, bool useOnlyRmField);

    Gv*
    createGv(std::uint8_t opcode, bool isQuadWord);

    Register
    mapDoubleWordRegisterIndex(std::uint8_t reg);

    Register
    mapQuadWordRegisterIndex(std::uint8_t reg);

    MemoryAddress32*
    createMemoryAddress32(Instruction* instruction);

    MemoryAddress32*
    createMemoryAddress16();

    void
    parseOneByteOpCode(List<Instruction*>& instructions, Instruction* instruction, uint8_t opcode);

    void
    parseTwoByteOpCode(uint8_t opcode, Instruction* instruction, List<Instruction*>& instructions);

    void
    parseThreeByteOpCode(uint8_t opcode, Instruction* instruction, List<Instruction*>& instructions);
};


}


#endif //ELET_X86PARSER_H
