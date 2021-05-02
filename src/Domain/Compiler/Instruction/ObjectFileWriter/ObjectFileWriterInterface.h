#ifndef ELET_OBJECTFILEWRITERINTERFACE_H
#define ELET_OBJECTFILEWRITERINTERFACE_H

#include <map>
#include <Foundation/Path.h>
#include <Domain/Compiler/Syntax/Instruction.h>
#include <Domain/Compiler/Compiler.h>


namespace elet::domain::compiler
{
    struct Symbol;
}

namespace elet::domain::compiler::instruction::output
{


struct RelocationOperand;


struct AssemblySegments
{
    List<Routine*>*
    text;

    std::uint64_t
    textSize;

    List<Utf8StringView*>*
    cstrings;

    std::size_t
    cstringSize;

    List<Symbol*>*
    symbols;

    std::size_t
    symbolSize;

    List<RelocationOperand*>*
    symbolicRelocations;

    List<RelocationOperand*>*
    relativeRelocations;
};


class ObjectFileWriterInterface
{

public:

    virtual
    void
    writeToFile(const Path& file, const AssemblySegments& segments) = 0;

};


}

#endif //ELET_OBJECTFILEWRITERINTERFACE_H
