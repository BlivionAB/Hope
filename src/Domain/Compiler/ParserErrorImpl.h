#ifndef ELET_PARSERERROR_IMPL_H
#define ELET_PARSERERROR_IMPL_H

#include "ParserError.h"


namespace elet::domain::compiler::ast
{
    template<typename T, typename... Args>
    T*
    ParserError::throwSyntaxErrorWithNode(const Syntax* syntax, Args... args)
    {
        _parser->skipToNextSemicolon();
        throw new T(_parser->_sourceFile, syntax, args...);
    }

    template<typename T, typename... Args>
    T*
    ParserError::throwSyntaxError(Args... args)
    {
        const ErrorNode* errorNode = createErrorNodeOnCurrentToken();
        _parser->skipToNextSemicolon();
        throw new T(_parser->_sourceFile, errorNode, args...);
    }
}


#endif // ELET_PARSERERROR_IMPL_H