//
// Created by tinganho on 2021-12-25.
//

#ifndef ELET_PE32BASELINEPARSER_H
#define ELET_PE32BASELINEPARSER_H


namespace elet::domain::compiler::test
{
    template<
        typename TAssemblyParser,
        typename TAssemblyPrinter,
        typename TOneOfInstructions
    >
    class Pe32BaselineParser : public ObjectFileBaselineParser
    {

    public:

        Pe32BaselineParser(List<uint8_t>& list, AssemblyTarget assemblyTarget);

        void
        parse() override;
    };
}

#endif //ELET_PE32BASELINEPARSER_H
