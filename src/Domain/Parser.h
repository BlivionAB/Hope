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
    class Compiler;
    struct ParsingTask;
}
namespace elet::domain::compiler::instruction
{
    class InstructionParser;
}

namespace elet::domain::compiler::ast
{

struct ParameterDeclarationList;
struct File;
struct Type;
struct Syntax;
struct FunctionDeclaration;
struct VariableDeclaration;
struct CallExpression;
struct ArgumentList;
struct InterfaceDeclaration;
struct EnumDeclaration;
struct AssemblyBlock;
struct DeclarationMetadata;
struct Tuple;
struct ImportStatement;
struct ExportStatement;
struct ModuleDeclaration;
struct EndStatement;
struct ModuleAccessUsage;
struct UseStatement;
struct NamedUsage;
struct ParameterDeclaration;
struct Expression;
struct Identifier;
struct FunctionBody;
struct LengthOfExpression;
struct ArrayLiteral;
struct AssemblyBody;
struct Punctuation;
enum class PunctuationType;
enum class SyntaxKind : std::uint8_t ;

    using Token = Scanner::Token;


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

    instruction::InstructionParser*
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

    EndStatement*
    parseEndStatement();

    UseStatement*
    parseUseStatement();

    NamedUsage*
    parseNamedUsageOnOpenBrace();

    ParameterDeclaration*
    parseParameterOnIdentifier();

    Expression*
    parsePropertyAccessOrCallExpression();

    Expression*
    createPropertyAccessExpressionOrCallExpressionFromPeek(Token peek);

    Expression*
    parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();

    Identifier*
    parseIdentifier();

    FunctionBody*
    parseFunctionBody();

    Expression*
    parseExpressionOnToken(Token token);

    Expression*
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

    Punctuation*
    createPunctuation(PunctuationType type);

    void
    skipNextToken();

    template<typename T>
    T*
    createSyntax(SyntaxKind kind);

    template<typename T>
    T*
    createDeclaration(SyntaxKind kind);

    template<typename T>
    T*
    createBlock(SyntaxKind kind);

    template<typename T>
    T*
    createNamedExpression(SyntaxKind kind);

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
    getParameterDisplay(ParameterDeclaration* parameter);

    bool
    hasEqualIdentifier(Identifier* id1, Identifier* id2);
};


}


#endif //ELET_PARSER_H
