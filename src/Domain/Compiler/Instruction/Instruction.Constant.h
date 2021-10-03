#ifndef ELET_INSTRUCTION_CONSTANT_H
#define ELET_INSTRUCTION_CONSTANT_H

#include <cstddef>
#include "Foundation/List.h"
#include "Instruction.h"

namespace elet::domain::compiler::instruction::output
{
    using namespace elet::foundation;


    struct Constant
    {
        RelocationPlaceholder
        relocationAddress;

        Constant():
            relocationAddress(0, 0, 0)
        { }
    };


    struct String : Constant
    {
        Utf8StringView
        value;

        String(Utf8StringView value):
            value(value),
            Constant()
        { }
    };
}

#include "Instruction.h"

#endif //ELET_INSTRUCTION_CONSTANT_H
