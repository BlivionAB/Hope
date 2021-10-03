#ifndef ELET_ASSEMBLYPARSER_H
#define ELET_ASSEMBLYPARSER_H


#include <cstdint>
#include <Foundation/List.h>
#include <Domain/Compiler/Instruction/TextWriter.h>


namespace elet::domain::compiler::test
{


using namespace elet::foundation;
using namespace elet::domain::compiler::instruction;


class AssemblyParser
{
protected:

    List<uint8_t>*
    _output;

    uint64_t
    _offset;

    uint64_t
    _size;

    uint64_t
    _cursor = 0;
};


}
#endif //ELET_ASSEMBLYPARSER_H
