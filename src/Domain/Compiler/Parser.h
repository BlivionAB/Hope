#ifndef ELET_PARSER_H
#define ELET_PARSER_H

#include <Foundation/HashTableMap.h>
#include <Foundation/Path.h>
#include "Scanner.h"
#include "Compiler.h"
#include "Syntax/Syntax.h"
#include "Domain/Compiler/Instruction/Parser.h"


namespace elet::domain::compiler
{
    class Compiler;
    struct Symbol;
    struct ParsingTask;
}


namespace elet::domain::compiler::instruction
{
    class InstructionParser;
}


namespace elet::domain::compiler::ast
{

struct ParameterDeclarationList;
struct SourceFile;
struct TypeAssignment;
struct Declaration;
struct Syntax;
struct FunctionDeclaration;
struct VariableDeclaration;
struct CallExpression;
struct ArgumentList;
struct InterfaceDeclaration;
struct ClassDeclaration;
struct EnumDeclaration;
struct AssemblyBlock;
struct ExternCBlock;
struct DeclarationMetadata;
struct Tuple;
struct EndStatement;
struct DomainAccessUsage;
struct UsingStatement;
struct NamedUsage;
struct ParameterDeclaration;
struct Expression;
struct Name;
struct StatementBlock;
struct LengthOfExpression;
struct ArrayLiteral;
struct AssemblyBody;
struct Punctuation;
struct Declaration;
struct DeclarationBlock;
struct AddressOfExpression;
struct ConstructorDeclaration;
struct AccessabilityLabel;
struct DomainDeclaration;
struct PropertyDeclaration;
enum class PunctuationType;
enum class AccessibilityType;
enum class SyntaxKind : std::uint8_t;

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

    static
    thread_local
    List<Symbol*>*
    symbols;

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

    static
    thread_local
    std::uint64_t
    _symbolOffset;

    Scanner*
    _scanner;

    std::map<Utf8String, SourceFile*>&
    _files;


    typedef std::map<Utf8StringView, std::variant<std::map<Utf8StringView, ast::Declaration*>*, void*>> AccessMap;
    AccessMap
    _domains;

    instruction::InstructionParser*
    _instructionParser = nullptr;

    Compiler*
    _compiler;

    TypeAssignment*
    parseType();

    Syntax*
    parseFunctionLevelStatement();

    Syntax*
    parseFileLevelDeclarations();

    Syntax*
    parseModuleLevelStatement();

    FunctionDeclaration*
    parseFunctionDeclaration();

//    FunctionMetadata*
//    parseFunctionMetadata();

    static
    bool
    isFunctionMetadataKeyword(Token token);

    VariableDeclaration*
    parseVariableDeclaration();

    void
    parseInterfaceDeclarationMetadata(InterfaceDeclaration* interface);

    CallExpression*
    parseCallExpressionOnName();

    ArgumentList*
    parseArgumentListOnOpenParen();

    ParameterListResult
    parseParameterList();

    ParameterListResult
    parseParameterListonOpenParen();

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

    DomainAccessUsage*
    parseDomainAccessUsageOnIdentifier();

//    ModuleDeclaration*
//    parseModuleDeclaration();

    UsingStatement*
    parseUsingStatement();

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

    AddressOfExpression*
    parseAddressOfExpression();

    ClassDeclaration*
    parseObjectDeclaration();

    PropertyDeclaration*
    parsePropertyDeclarationOnIdentifier(Name* name);

    ConstructorDeclaration*
    parseConstructorDeclaration();

    Name*
    parseName();

    StatementBlock*
    parseFunctionBody();

    StatementBlock*
    parseFunctionBodyOnOpenBrace();

    Expression*
    parseExpressionOnToken(Token token);

    DomainDeclaration*
    parseDomainDeclaration();

    Syntax* // Block or Declaration
    parseDomainLevelDeclaration();

    AccessabilityLabel*
    parseAccessabilityLabel(AccessibilityType);

    DeclarationBlock*
    parseDeclarationBlock();

    void
    addSymbolToSourceFile(Syntax* statement, SourceFile* sourceFile);

    List<CallExpression*>*
    parseInitializationList();

    Expression*
    parseExpression();

    LengthOfExpression*
    parseLengthOfExpression();

    ExternCBlock*
    parseExternBlock();

    List<Declaration*>
    parseExternCBlockLevelDeclarations();

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
    createTypeDeclaration(SyntaxKind kind);

    template<typename T>
    T*
    createBlock(SyntaxKind kind);

    template<typename T>
    T*
    createNamedExpression(SyntaxKind kind);

    template<typename T>
    void
    finishSyntax(T* syntax);

    template<typename T>
    void
    finishDeclaration(T* declaration);

    template<typename T>
    void
    finishTypeDeclaration(T* declaration);

    void
    expectToken(Token expected);

    void
    assertToken(Token target, Token expected);

    Utf8StringView
    getTokenValue();

    Name*
    createName();

    Utf8String
    getParameterDisplay(ParameterDeclaration* parameter);

    bool
    hasEqualIdentifier(Name* id1, Name* id2);


    static
    void
    addSymbol(Declaration* definition);

    static
    void
    addTypeSymbol(Declaration* declaration);
};


}


#endif //ELET_PARSER_H
