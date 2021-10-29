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

        template<typename... Args>
        error::SyntaxError*
        throwSyntaxError(Utf8String message, Args... args);

        template<typename... Args>
        error::SyntaxError*
        throwSyntaxError(const Syntax* syntax, Utf8String message, Args... args);

        ErrorNode*
        createErrorNodeOnCurrentToken();

        error::SyntaxError*
        createExpectedTokenError(Token token);
    };
}


#include "ParserErrorImpl.h"

#endif //ELET_PARSERERROR_H
