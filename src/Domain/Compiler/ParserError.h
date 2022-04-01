#ifndef ELET_PARSERERROR_H
#define ELET_PARSERERROR_H


#include "Parser.h"


namespace elet::domain::compiler::ast
{
    class ParserError
    {
    public:

        ParserError(Parser* parser);

        Parser*
        _parser;

        template<typename T, typename... Args>
        T*
        throwSyntaxErrorWithNode(const Syntax* syntax, Args... args);

        template<typename T, typename... Args>
        T*
        throwSyntaxError(Args... args);

        ErrorNode*
        createErrorNodeOnCurrentToken();

        error::SyntaxError*
        createExpectedTokenError(Token token);
    };
}


#include "ParserErrorImpl.h"

#endif //ELET_PARSERERROR_H
