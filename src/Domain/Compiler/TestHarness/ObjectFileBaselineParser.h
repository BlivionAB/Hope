#ifndef ELET_OBJECTFILEBASELINEPARSER_H
#define ELET_OBJECTFILEBASELINEPARSER_H


namespace elet::domain::compiler::test
{
    template<
        typename TAssemblyParser,
        typename TAssemblyBaselinePrinter
    >
    class ObjectFileBaselineParser
    {

    public:

        ObjectFileBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget);

        virtual
        void
        parse() = 0;

    protected:

        List<uint8_t>&
        _list;

        uint64_t
        _offset;

        AssemblyTarget
        _assemblyTarget;

        TAssemblyParser*
        _assemblyParser;

        TAssemblyBaselinePrinter*
        _assemblyBaselinePrinter;
    };
}


#include "ObjectFileBaselineParserImpl.h"


#endif //ELET_OBJECTFILEBASELINEPARSER_H
