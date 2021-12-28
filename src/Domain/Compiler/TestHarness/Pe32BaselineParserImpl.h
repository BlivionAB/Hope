//
// Created by tinganho on 2021-12-25.
//

#include "Pe32BaselineParser.h"

namespace elet::domain::compiler::test
{
    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    Pe32BaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::Pe32BaselineParser(List <uint8_t>& list, AssemblyTarget assemblyTarget):
        ObjectFileBaselineParser<TAssemlbyParser>(list, assemblyTarget)
    {

    }


    template<typename TAssemblyParser, typename TAssemblyPrinter, typename TOneOfInstructions>
    void
    Pe32BaselineParser<TAssemblyParser, TAssemblyPrinter, TOneOfInstructions>::parse()
    {
        _assemblyBaselinePrinter->list = &_list;
        MachHeader64* machHeader = reinterpret_cast<MachHeader64*>(&_list[0]);
        if (machHeader->magic != MACHO_MAGIC_64)
        {
            throw std::runtime_error("machHeader magic is wrong.");
        }
    }
}