#ifndef ELET_INSTRUCTION_CONSTANT_H
#define ELET_INSTRUCTION_CONSTANT_H

#include <cstddef>
#include "Foundation/List.h"

namespace elet::domain::compiler::instruction::output
{

using namespace elet::foundation;


struct Constant
{
    List<std::size_t>
    relocationAddress;
};


struct CString : Constant
{
    Utf8StringView
    value;

    CString(Utf8StringView value):
        value(value)
    { }
};

}
#endif //ELET_INSTRUCTION_CONSTANT_H
