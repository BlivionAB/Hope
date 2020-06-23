#include "Parser.h"
#include "Exceptions.h"

using Token = Scanner::Token;

Parser::Parser(const Scanner& tokenIterator):
        _scanner(tokenIterator)
{

}

Syntax*
Parser::parseStatement()
{
    Token token = _scanner.takeNextToken();
    switch (token)
    {
        case Token::FunctionKeyword:
            parseFunctionDeclaration();
        case Token::IfKeyword:
            return nullptr; // Todo here.
    }
}

FunctionDeclaration*
Parser::parseFunctionDeclaration()
{
    FunctionDeclaration* _function = createSyntax<FunctionDeclaration>();
    if (!expectIdentifier())
    {
        _function->name = getTokenValue();
    }
    _function->body = parseFunctionBody();
    finishSyntax<FunctionDeclaration>(_function);
    return _function;
}

FunctionBody*
Parser::parseFunctionBody()
{
    expectToken(Token::OpenBrace);
    FunctionBody* body = createSyntax<FunctionBody>();
    std::vector<Syntax*> statements;
    while (peekNextToken() != Token::CloseBrace)
    {
        statements.push_back(parseStatement());
    }
    body->statements = statements;
    expectToken(Token::CloseBrace);
    return body;
}

template<typename T>
T*
Parser::createSyntax()
{
    return new T();
}

template<typename T>
void
Parser::finishSyntax(T* syntax)
{
    syntax->end = _scanner.getPosition();
}

bool
Parser::expectIdentifier()
{
    Token token = _scanner.takeNextToken();
    return token != Token::Identifier;
}

Utf8StringView
Parser::getTokenValue()
{
    return _scanner.getTokenValue();
}

void
Parser::expectToken(Token token)
{
    Token result = _scanner.takeNextToken();
    if (result != token)
    {
        throw UnexpectedToken("Expected token", token);
    }
}

Token
Parser::peekNextToken()
{
    return _scanner.peekNextToken();
}
