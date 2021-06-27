#ifndef ELET_AARCH64PARSER_H
#define ELET_AARCH64PARSER_H


#include "../AssemblyParser.h"
#include "AarchTypes.h"

namespace elet::domain::compiler::test::aarch
{


class Aarch64Parser
{

public:

    List<Instruction>
    parse(List<std::uint8_t>& output, size_t offset, size_t size);

    uint8_t
    getByte(Instruction* instruction);

    uint32_t
    getDoubleWord(Instruction* instruction);
};


}

#endif //ELET_AARCH64PARSER_H
