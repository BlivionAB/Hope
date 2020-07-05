#ifndef ELET_PARSER_H
#define ELET_PARSER_H

#include <Foundation/HashTableMap.h>
#include <Foundation/Path.h>
#include "Scanner.h"
#include "Compiler.h"
#include "Syntax.h"

using Token = Scanner::Token;

struct SymbolLocation
{
    std::size_t
    start;

    std::size_t
    end;
};

class Compiler;

class Parser
{

public:

    Parser(Compiler* compiler);

    List<Syntax*>
    parse(const char* sourceStart, const char* sourceEnd, const Path *currentDirectory);

private:

    static
    thread_local
    Path*
    _currentDirectory;

    Scanner*
    _scanner;

    char*
    _lastStatement;

    Compiler*
    _compiler;

    Type*
    parseType();

    Syntax*
    parseStatement();

    FunctionDeclaration*
    parseFunctionDeclaration();

    CallExpression*
    parseCallExpressionOnIdentifier();

    ArgumentList*
    parseArgumentListOnOpenParen();

    ParameterList*
    parseParameterList();

    InterfaceDeclaration*
    parseInterfaceDeclaration();

    EnumDeclaration*
    parseEnumDeclaration();

    ImportStatement*
    parseImportStatement();

    ExportStatement*
    parseExportStatement();

    ModuleAccessUsage*
    parseModuleAccessUsageOnIdentifier();

    ModuleDeclaration*
    parseModuleDeclaration();

    EndStatement*
    parseEndStatement();

    UseStatement*
    parseUseStatement();

    NamedUsage*
    parseNamedUsageOnOpenBrace();

    Parameter*
    parseParameterOnIdentifier();

    Expression*
    parsePropertyAccessOrCallExpression();

    Identifier*
    parseIdentifier();

    FunctionBody*
    parseFunctionBody();

    Expression*
    parseExpression();

    Token
    peekNextToken();

    Token
    takeNextToken();

    Punctuation*
    createPunctuation(PunctuationType type);

    void
    skipNextToken();

    template<typename T>
    T*
    createSyntax(SyntaxKind kind);

    template<typename T>
    void
    finishSyntax(T* syntax);

    void
    expectToken(Token token);

    Utf8StringView
    getTokenValue();

    Identifier*
    createIdentifer();

    bool
    hasEqualIdentifier(Identifier* id1, Identifier* id2);
};

#endif //ELET_PARSER_H
