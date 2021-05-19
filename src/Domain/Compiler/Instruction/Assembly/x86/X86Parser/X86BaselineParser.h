#ifndef ELET_X86BASELINEPARSER_H
#define ELET_X86BASELINEPARSER_H


#include <cstdint>
#include "X86Types.h"
#include "Foundation/List.h"


namespace elet::domain::compiler::instruction::x86
{


using namespace elet::foundation;


class X86BaselineParser
{

public:

    List<Instruction*>*
    parse(List<std::uint8_t>* output);

private:

    List<std::uint8_t>*
    _output;

    std::size_t
    _cursor = 0;

    std::uint8_t
    getByte();

    std::array<std::uint8_t, 4>
    getDoubleWord();

    x86::Ev*
    createEv(std::uint8_t modrmByte, Instruction* instruction);

    x86::Gv*
    createGv(std::uint8_t opcode, bool isQuadWord);

    Register
    mapDoubleWordRegisterIndex(std::uint8_t reg);


    Register
    mapQuadWordRegisterIndex(std::uint8_t reg);

    MemoryAddress32*
    createMemoryAddress32();

    MemoryAddress32*
    createMemoryAddress16();
};


}


#endif //ELET_X86BASELINEPARSER_H
