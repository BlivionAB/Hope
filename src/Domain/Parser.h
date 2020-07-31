#ifndef ELET_PARSER_H
#define ELET_PARSER_H

#include <Foundation/HashTableMap.h>
#include <Foundation/Path.h>
#include "Scanner.h"
#include "Compiler.h"
#include "Syntax.h"
#include <Domain/Instruction/Parser.h>


namespace elet::domain::compiler
{


using Token = Scanner::Token;


struct ParsingTask;


struct ParseResult
{
    List<Syntax*>
    statements;

    ParsingTask*
    pendingParsingTask;
};


struct ParameterListResult
{
    ParameterDeclarationList*
    parameterList;

    Utf8String
    display;
};


class Compiler;

class Parser
{

public:

    Parser(Compiler* compiler);

    ParseResult
    performWork(const ParsingTask& task);

    void
    seek(const BaseScanner::Location& location);

private:

    static
    thread_local
    Path*
    _currentDirectory;

    static
    thread_local
    const char*
    _lastStatementLocationStart;

    static
    thread_local
    const char*
    _lastStatementLocationEnd;

    Scanner*
    _scanner;

    std::map<Utf8String, File*>&
    _files;

    InstructionParser*
    _instructionParser = nullptr;

    Compiler*
    _compiler;

    Type*
    parseType();

    Syntax*
    parseFunctionLevelStatement();

    Syntax*
    parseFileLevelStatement();

    Syntax*
    parseModuleLevelStatement();

    FunctionDeclaration*
    parseFunctionDeclaration();

    VariableDeclaration*
    parseVariableDeclaration();

    CallExpression*
    parseCallExpressionOnIdentifier();

    ArgumentList*
    parseArgumentListOnOpenParen();

    ParameterListResult
    parseParameterList();

    InterfaceDeclaration*
    parseInterfaceDeclaration();

    EnumDeclaration*
    parseEnumDeclaration();

    AssemblyBlock*
    parseAssemblyBlock();

    DeclarationMetadata*
    parseDeclarationMetadata();

    Tuple*
    parseTuple();

    ImportStatement*
    parseImportStatement();

    ExportStatement*
    parseExportStatement();

    ModuleAccessUsage*
    parseModuleAccessUsageOnIdentifier();

    ModuleDeclaration*
    parseModuleDeclaration();

    compiler::EndStatement*
    parseEndStatement();

    UseStatement*
    parseUseStatement();

    NamedUsage*
    parseNamedUsageOnOpenBrace();

    compiler::ParameterDeclaration*
    parseParameterOnIdentifier();

    compiler::Expression*
    parsePropertyAccessOrCallExpression();

    compiler::Expression*
    createPropertyAccessExpressionOrCallExpressionFromPeek(Token peek);

    compiler::Expression*
    parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();

    Identifier*
    parseIdentifier();

    FunctionBody*
    parseFunctionBody();

    compiler::Expression*
    parseExpression();

    LengthOfExpression*
    parseLengthOfExpression();

    ArrayLiteral*
    parseArrayLiteral();

    AssemblyBody*
    parseAssemblyBody();

    Token
    peekNextToken();

    Token
    takeNextToken();

    compiler::Punctuation*
    createPunctuation(compiler::PunctuationType type);

    void
    skipNextToken();

    template<typename T>
    T*
    createSyntax(compiler::SyntaxKind kind);

    template<typename T>
    T*
    createDeclaration(compiler::SyntaxKind kind);

    template<typename T>
    T*
    createBlock(compiler::SyntaxKind kind);

    template<typename T>
    T*
    createNamedExpression(compiler::SyntaxKind kind);

    template<typename T>
    void
    finishSyntax(T* syntax);

    void
    expectToken(Token token);

    Utf8StringView
    getTokenValue();

    Identifier*
    createIdentifer();

    Utf8String
    getParameterDisplay(compiler::ParameterDeclaration* parameter);

    bool
    hasEqualIdentifier(Identifier* id1, Identifier* id2);
};


}


#endif //ELET_PARSER_H
