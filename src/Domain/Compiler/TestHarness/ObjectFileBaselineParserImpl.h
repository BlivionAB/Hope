#include "./ObjectFileBaselineParser.h"

#ifndef ELET_OBJECTFILEBASELINEPARSERIMPL_H
#define ELET_OBJECTFILEBASELINEPARSERIMPL_H

namespace elet::domain::compiler::test
{
    template<typename TAssemblyParser, typename TAssemblyBaselinePrinter>
    ObjectFileBaselineParser<TAssemblyParser, TAssemblyBaselinePrinter>::ObjectFileBaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget):
        _list(list),
        _assemblyTarget(assemblyTarget),
        _assemblyParser(new TAssemblyParser()),
        _assemblyBaselinePrinter(new TAssemblyBaselinePrinter())
    {

    }
}


#endif //ELET_OBJECTFILEBASELINEPARSERIMPL_H
