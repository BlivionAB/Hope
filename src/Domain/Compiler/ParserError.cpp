
#include "ParserError.h"

namespace elet::domain::compiler::ast
{
    ParserError::ParserError(Parser* parser):
        _parser(parser)
    {

    }


    ErrorNode*
    ParserError::createErrorNodeOnCurrentToken()
    {
        ErrorNode* errorNode = _parser->createSyntax<ErrorNode>(SyntaxKind::ErrorNode);
        _parser->finishSyntax(errorNode);
        return errorNode;
    }


    error::SyntaxError*
    ParserError::createExpectedTokenError(Token token)
    {
        return new error::SyntaxError(createErrorNodeOnCurrentToken(),
            _parser->_sourceFile,
            "Expected '{0}', instead got '{1}'.",
            eletTokenToString.get(token),
            _parser->getTokenValue().toString());
    }
}