#ifndef ELET_PARSER_H
#define ELET_PARSER_H

#include <Foundation/HashTableMap.h>
#include "Scanner.h"
#include "Syntax.h"

using Token = Scanner::Token;

struct SymbolLocation
{
    std::size_t
    start;

    std::size_t
    end;
};

class Parser
{

public:

    Parser(const Scanner& tokenIterator);

private:

    Scanner
    _scanner;

    Syntax*
    parseStatement();

    FunctionDeclaration*
    parseFunctionDeclaration();

    FunctionBody*
    parseFunctionBody();

    Token
    peekNextToken();

    template<typename T>
    T*
    createSyntax();

    template<typename T>
    void
    finishSyntax(T* syntax);

    bool
    expectIdentifier();

    void
    expectToken(Token token);

    Utf8StringView
    getTokenValue();
};

#endif //ELET_PARSER_H
