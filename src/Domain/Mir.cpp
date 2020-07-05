#include "Mir.h"

Mir::Mir(List<Syntax*>& statements):
    _statements(statements)
{

}

void
Mir::convertStatementsToControlFlowGraph()
{
    for (const Syntax* statement : _statements)
    {
        switch (statement->kind)
        {
            case SyntaxKind::FunctionDeclaration:
                break;
        }
    }
}

