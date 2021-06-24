#ifndef ELET_DIFFPRINTER_H
#define ELET_DIFFPRINTER_H


#include <unordered_map>
#include "./MyersDiff.h"
#include <Domain/Compiler/Instruction/TextWriter.h>

namespace elet::domain::compiler::test
{

using namespace elet::domain::compiler::instruction;

class DiffPrinter
{

public:

    DiffPrinter();

    std::string
    print(const List<Edit>& diffs);

private:

    TextWriter
    _tw;
};


}


#endif //ELET_DIFFPRINTER_H
