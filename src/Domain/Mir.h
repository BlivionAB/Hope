#ifndef ELET_MIR_H
#define ELET_MIR_H


#include "Syntax.h"

class Mir
{
public:

    Mir(List<Syntax*>& statements);

    void
    convertStatementsToControlFlowGraph();

    void
    convertFunctionDeclarationToControlFlowGraph();

private:

    List<Syntax*>&
    _statements;
};


#endif //ELET_MIR_H
