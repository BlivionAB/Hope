#ifndef ELET_SYNTAX_STATEMENT_H
#define ELET_SYNTAX_STATEMENT_H


#include "Syntax.h"


namespace elet::domain::compiler::ast
{
    struct StatementBlock;
    struct Expression;


    struct IfStatement : Syntax
    {
        Expression*
        condition;

        StatementBlock*
        body;
    };


    struct ReturnStatement : Syntax
    {
        Expression*
        expression;

        Type*
        expectedType;
    };
}


#endif //ELET_SYNTAX_STATEMENT_H
