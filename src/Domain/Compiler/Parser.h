#ifndef ELET_PARSER_H
#define ELET_PARSER_H

#include <filesystem>
#include <Foundation/HashTableMap.h>
#include <Domain/Compiler/Syntax/Syntax.Statement.h>
#include "Scanner.h"
#include "Syntax/Syntax.h"
#include "Domain/Compiler/Error/Error.h"
#include "Domain/Compiler/Instruction/Parser.h"


namespace fs = std::filesystem;


namespace elet::domain::compiler
{
    namespace instruction
    {
        class InstructionParser;
    }
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
    struct EndStatement;
    struct DomainAccessUsage;
    struct UsingStatement;
    struct UsageClause;
    struct ParameterDeclaration;
    struct Expression;
    struct Name;
    struct StatementBlock;
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
    class ParserError;
    enum class PunctuationType;
    enum class AccessibilityType;
    enum class SyntaxKind : std::uint8_t;


    namespace error
    {
        struct SyntaxError;
    }


    struct ParsingTask
    {
        ast::SourceFile*
        sourceFile;

//        bool
//        isEndOfFile;

        ParsingTask(
            ast::SourceFile*
            sourceFile

//            bool
//            endOfFile
        ):
            sourceFile(sourceFile)
//            isEndOfFile(endOfFile)
        { }
    };


    struct PendingParsingTask
    {
        const char*
        pendingBlock;

        ParsingTask*
        task;

        PendingParsingTask(const char* pendingBlock, ParsingTask* task):
            pendingBlock(pendingBlock),
            task(task)
        { }
    };

    struct ParseResult
    {
        List<Syntax*>
        statements;

        List<error::SyntaxError*>
        syntaxErrors;

        List<error::LexicalError*>
        lexicalErrors;
    };


    struct ParameterListResult
    {
        ParameterDeclarationList*
        parameterList;

        Utf8String
        display;
    };


    typedef std::map<Utf8StringView, std::variant<std::map<Utf8StringView, Declaration*>*, void*>> DomainDeclarationMap;


    class Parser final
    {
    public:

        friend class ParserError;

        Parser(std::map<std::string, SourceFile*>& files);

        ParseResult
        performWork(SourceFile* sourceFile);

        void
        seek(const TextScanner::Location& location);

        static
        thread_local
        List<Symbol*>*
        symbols;

        DomainDeclarationMap
        domainDeclarationMap;

        template<typename T>
        T*
        createSyntax(SyntaxKind kind);

        template<typename T>
        void
        finishSyntax(T* syntax);

        Utf8StringView
        getTokenValue();

    private:

        ParserError*
        _error;

        const
        SourceFile*
        _sourceFile;

        List<error::SyntaxError*>
        _syntaxErrors;

        List<error::LexicalError*>
        _lexicalErrors;

//        static
//        thread_local
//        fs::path*
//        _currentDirectory;

        static
        thread_local
        const char*
        _lastStatementLocationEnd;

        static
        thread_local
        std::uint64_t
        _symbolOffset;

        bool
        _isInCBlock = false;

        Scanner*
        _scanner;

        std::map<std::string, SourceFile*>&
        _files;

        DomainDeclaration*
        _currentDomain = nullptr;

        instruction::InstructionParser*
        _instructionParser = nullptr;

        TypeAssignment*
        parseType();

        Syntax*
        parseFunctionLevelStatement();

        Syntax*
        parseFileLevelDeclarations();

        FunctionDeclaration*
        parseFunctionDeclaration();

        void
        skipToNextSemicolon();

        static
        bool
        isFunctionMetadataKeyword(Token token);

        VariableDeclaration*
        parseVariableDeclaration();

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

        ParenExpression*
        parseParenExpression();

    //    DomainAccessUsage*
    //    parseDomainAccessUsageOnIdentifier();

    //    ModuleDeclaration*
    //    parseModuleDeclaration();

        UsingStatement*
        parseUsingStatement();

        UsageClause*
        parseUsageClauseOnOpenBrace();

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
        parseStatementBlock();

        StatementBlock*
        parseFunctionBodyOnOpenBrace();

        Expression*
        parseExpressionOnToken(Token token);

        DomainDeclaration*
        parseDomainDeclaration();

        Syntax* // Block or Declaration
        parseDomainLevelStatements();

        AccessabilityLabel*
        parseAccessabilityLabel(AccessibilityType);

        DeclarationBlock*
        parseDeclarationBlock();

        void
        addSymbolToSourceFile(Syntax* statement, SourceFile* sourceFile);

        List<CallExpression*>*
        parseInitializationList();

        Expression*
        parseExpression(bool skipNextOperator);

        Expression*
        parseRightHandSideOfBinaryExpression(unsigned int previousOperatorPrecedence);

        uint8_t
        getOperatorPrecedence(Token token) const;

        bool isBinaryOperator(Token token) const;

        ExternCBlock*
        parseExternCBlock();

        List<Declaration*>
        parseExternCBlockLevelDeclarations();

        ArrayLiteral*
        parseArrayLiteral();

        AssemblyBody*
        parseAssemblyBody();

        Token
        peekNextToken(bool includeWhitespaceToken);

        Token
        takeNextToken();

        Token
        takeNextToken(bool includeWhitespaceToken);

        Punctuation*
        createPunctuation(PunctuationType type);

        void
        skipNextToken();

        template<typename T>
        T*
        createDeclaration(const SyntaxKind kind);

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
        finishDeclaration(T* declaration);

        template<typename T>
        void
        finishTypeDeclaration(T* declaration);

        void
        expectToken(Token expected);

        void
        assertToken(Token target, Token expected);

        Name*
        createName();

        Utf8String
        getParameterDisplay(ParameterDeclaration* parameter);

        void
        addSymbol(Declaration* definition);

        static
        void
        addTypeSymbol(Declaration* declaration);

        IfStatement*
        parseIfStatement();

        BooleanLiteral*
        createBooleanLiteral(bool value);

        BinaryOperatorKind
        getBinaryOperatorKind(Token token);

        ReturnStatement*
        parseReturnStatement();

        uint64_t
        parseBinaryLiteral(const BinaryLiteral* binaryLiteral, bool isNegative) const;

        uint64_t
        parseDecimalLiteral(const DecimalLiteral* decimalLiteral, bool isNegative) const;

        uint64_t
        parseHexadecimalLiteral(const HexadecimalLiteral* hexadecimalLiteral, bool isNegative) const;

        bool
        isIntegerSuffix(Token token);

        IntegerLiteral*
        createIntegerLiteral(Token& token);

        IntegerLiteral*
        createIntegerLiteral(Token& token, bool isNegative);

        unsigned int
        getDigitsLength(const char* end, const char* start) const;

        IntegerLiteral*
        createNegativeIntegerLiteral();

//        DeclarationDecoration*
//        parseDecoration();
    CharacterLiteral*
    createCharacterLiteral();

    Expression*
    parseBinaryExpression(Expression* expression, Token& peek);
};
}

#include "Parser.Impl.h"
#include "ParserError.h"


#endif //ELET_PARSER_H
