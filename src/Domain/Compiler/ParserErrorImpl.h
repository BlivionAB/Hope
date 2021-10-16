#ifndef ELET_PARSERERROR_IMPL_H
#define ELET_PARSERERROR_IMPL_H

#include "ParserError.h"


namespace elet::domain::compiler::ast
{
    template<typename... Args>
    error::SyntaxError*
    ParserError::createSyntaxError(Utf8String message, Args... args)
    {
        ErrorNode* errorNode = createErrorNodeOnCurrentToken();
        return new error::SyntaxError(errorNode, message.toString(), args...);
    }


    template<typename... Args>
    error::SyntaxError*
    ParserError::createSyntaxError(const Syntax* syntax, Utf8String message, Args... args)
    {
        return new error::SyntaxError(syntax, message.toString(), args...);
    }
}


#endif // ELET_PARSERERROR_IMPL_H