#ifndef ELET_OBJECTFILEWRITERINTERFACE_H
#define ELET_OBJECTFILEWRITERINTERFACE_H

#include <map>
#include <Foundation/Path.h>
#include <Domain/Instruction/Instruction.h>


namespace elet::domain::compiler::instruction::output
{


struct AssemblySegments
{
    std::map<Routine*, List<std::uint8_t>*>*
    text;

    List<Utf8StringView>*
    symbols;
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
