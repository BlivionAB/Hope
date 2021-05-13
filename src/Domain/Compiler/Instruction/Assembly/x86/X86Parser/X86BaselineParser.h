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

    std::uint32_t
    getDoubleWord();

    x86::Ev*
    createEv(std::uint8_t modrmByte);

    x86::Gv*
    createGv(std::uint8_t opcode);

    Register
    mapRegIndex(std::uint8_t reg);
};


}


#endif //ELET_X86BASELINEPARSER_H
