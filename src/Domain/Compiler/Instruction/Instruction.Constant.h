#ifndef ELET_INSTRUCTION_CONSTANT_H
#define ELET_INSTRUCTION_CONSTANT_H

#include <cstddef>
#include "Foundation/List.h"

namespace elet::domain::compiler::instruction::output
{

using namespace elet::foundation;


struct Constant
{
    std::size_t
    relocationAddress;

    Constant():
        relocationAddress(0)
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
#endif //ELET_INSTRUCTION_CONSTANT_H
