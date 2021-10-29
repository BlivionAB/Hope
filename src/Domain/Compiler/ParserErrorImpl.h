#ifndef ELET_PARSERERROR_IMPL_H
#define ELET_PARSERERROR_IMPL_H

#include "ParserError.h"


namespace elet::domain::compiler::ast
{
    template<typename... Args>
    error::SyntaxError*
    ParserError::throwSyntaxError(Utf8String message, Args... args)
    {
        ErrorNode* errorNode = createErrorNodeOnCurrentToken();
        _parser->skipToNextSemicolon();
        throw new error::SyntaxError(errorNode, _parser->_sourceFile, message.toString(), args...);
    }


    template<typename... Args>
    error::SyntaxError*
    ParserError::throwSyntaxError(const Syntax* syntax, Utf8String message, Args... args)
    {
        _parser->skipToNextSemicolon();
        throw new error::SyntaxError(syntax, _parser->_sourceFile, message.toString(), args...);
    }
}


#endif // ELET_PARSERERROR_IMPL_H