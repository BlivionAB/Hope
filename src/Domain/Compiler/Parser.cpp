#include "Parser.h"
#include "Exceptions.h"
#include <filesystem>


namespace fs = std::filesystem;


namespace elet::domain::compiler::ast
{


thread_local
fs::path*
Parser::_currentDirectory = nullptr;


thread_local
const char*
Parser::_lastStatementLocationStart = nullptr;


thread_local
const char*
Parser::_lastStatementLocationEnd = nullptr;


thread_local
List<Symbol*>*
Parser::symbols = nullptr;


thread_local
std::uint64_t
Parser::_symbolOffset = 0;


Parser::Parser(Compiler* compiler):
    _compiler(compiler),
    _files(compiler->files),
    _scanner(nullptr)
{

}


ParseResult
Parser::performWork(const ParsingTask& task)
{
    _lastStatementLocationStart = const_cast<char*>(task.sourceStart);
    Utf8StringView sourceContent(task.sourceStart, task.sourceEnd);
    Scanner scanner(sourceContent);
    _currentDirectory = const_cast<fs::path*>(task.sourceDirectory);
    _scanner = &scanner;
    List<Syntax*> statements;
    ParsingTask* pendingParsingTask = nullptr;
    while (true)
    {
        try
        {
            Syntax* statement = parseFileLevelDeclarations();
            if (!statement)
            {
                break;
            }
            addSymbolToSourceFile(statement, task.sourceFile);
            _lastStatementLocationStart = statement->end;
            statements.add(statement);
        }
        catch (UnexpectedEndOfFile &ex)
        {
            pendingParsingTask = new ParsingTask(_lastStatementLocationStart, _lastStatementLocationEnd, _currentDirectory, task.sourceFile, task.isEndOfFile);
        }
    }
    return { statements, pendingParsingTask };
}


void
Parser::addSymbolToSourceFile(Syntax* statement, SourceFile* sourceFile)
{
    if (statement->labels & LABEL__DECLARATION)
    {
        auto declaration = reinterpret_cast<Declaration*>(statement);
        if (declaration->kind == SyntaxKind::DomainDeclaration)
        {
            auto domain = reinterpret_cast<DomainDeclaration*>(declaration);
            for (auto decl : domain->block->declarations)
            {
                addSymbolToSourceFile(decl, sourceFile);
            }
        }
        if (declaration->symbol)
        {
            sourceFile->symbols.insert({ declaration->symbol->name, declaration });
            declaration->sourceFile = sourceFile;
        }
    }
}

DeclarationBlock*
Parser::parseDeclarationBlock()
{
    auto block = createSyntax<DeclarationBlock>(SyntaxKind::DeclarationBlock);
    AccessibilityType accessibility = AccessibilityType::Private;
    while (true)
    {
        Syntax* statement = parseDomainLevelStatements();
        if (!statement)
        {
            break;
        }
        switch (statement->kind)
        {
            case SyntaxKind::AccessabilityLabel:
            {
                auto label = reinterpret_cast<AccessabilityLabel*>(statement);
                accessibility = label->__type;
                break;
            }
            case SyntaxKind::FunctionDeclaration:
            {
                auto declaration = reinterpret_cast<Declaration*>(statement);
                declaration->accessability = accessibility;
                block->symbols.insert({ declaration->name->name, declaration });
                break;
            }
            case SyntaxKind::ExternCBlock:
            {
                auto cblock = reinterpret_cast<ExternCBlock*>(statement);
                for (const auto& decl : cblock->declarations)
                {
                    decl->accessability = accessibility;
                    block->symbols.insert({ decl->name->name, decl });
                }
            }
            default:;
        }
        block->declarations.add(statement);
        _lastStatementLocationStart = statement->end;
    }
    return block;
}

Syntax*
Parser::parseDomainLevelStatements()
{
    Token token = _scanner->takeNextToken();
    switch (token)
    {
        case Token::UsingKeyword:
            return parseUsingStatement();
        case Token::PublicKeyword:
        {
            expectToken(Token::Colon);
            AccessabilityLabel* label = createSyntax<AccessabilityLabel>(SyntaxKind::AccessabilityLabel);
            label->__type = AccessibilityType::Public;
            finishSyntax(label);
            return label;
        }
        case Token::PrivateKeyword:
        {
            expectToken(Token::Colon);
            AccessabilityLabel* label = createSyntax<AccessabilityLabel>(SyntaxKind::AccessabilityLabel);
            label->__type = AccessibilityType::Private;
            finishSyntax(label);
            return label;
        }
        case Token::ExternKeyword:
            return parseExternCBlock();
        case Token::DomainKeyword:
            return parseDomainDeclaration();
        case Token::ClassKeyword:
            return parseObjectDeclaration();
        case Token::InterfaceKeyword:
            return parseInterfaceDeclaration();
        case Token::EnumKeyword:
            return parseEnumDeclaration();
        case Token::FunctionKeyword:
            return parseFunctionDeclaration();
        case Token::EndOfFile:
        case Token::CloseBrace:
            return nullptr;
        default:
            throw UnknownFileLevelStatement();
    }
}


Syntax*
Parser::parseFileLevelDeclarations()
{
    Token token = _scanner->takeNextToken();
    switch (token)
    {
        case Token::DomainKeyword:
            return parseDomainDeclaration();
        case Token::ClassKeyword:
            return parseObjectDeclaration();
        case Token::InterfaceKeyword:
            return parseInterfaceDeclaration();
        case Token::EnumKeyword:
            return parseEnumDeclaration();
        case Token::UsingKeyword:
            return parseUsingStatement();
        case Token::FunctionKeyword:
            return parseFunctionDeclaration();
        case Token::EndOfFile:
        case Token::CloseBrace:
            return nullptr;
        default:
            throw UnknownFileLevelStatement();
    }
}


Syntax*
Parser::parseModuleLevelStatement()
{
    Token token = _scanner->takeNextToken();
    switch (token)
    {
        case Token::ClassKeyword:
            return parseObjectDeclaration();
        case Token::InterfaceKeyword:
            return parseInterfaceDeclaration();
        case Token::EnumKeyword:
            return parseEnumDeclaration();
        case Token::UsingKeyword:
            return parseUsingStatement();
        case Token::FunctionKeyword:
            return parseFunctionDeclaration();
        case Token::EndOfFile:
            return nullptr;
        default:
            throw UnknownFileLevelStatement();
    }
}


Syntax*
Parser::parseFunctionLevelStatement()
{
    Token token = _scanner->takeNextToken();
    switch (token)
    {
        case Token::VarKeyword:
            return parseVariableDeclaration();
        case Token::ReturnKeyword:
            return parseReturnStatement();
        case Token::AssemblyKeyword:
            return parseAssemblyBlock();
        case Token::IfKeyword:
            return parseIfStatement();
        case Token::Identifier:
        {
            Token peek = peekNextToken();
            switch (peek)
            {
                case Token::OpenParen:
                   return parseCallExpressionOnName();
                case Token::Colon:
                    return nullptr;
                case Token::Dot:
                {
                    PropertyAccessExpression *propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
                    propertyAccessExpression->name = createName();
                    skipNextToken();
                    propertyAccessExpression->dot = createPunctuation(PunctuationType::Dot);
                    propertyAccessExpression->expression = parsePropertyAccessOrCallExpression();
                    return propertyAccessExpression;
                }
            }
        }
        case Token::EndOfFile:
            return nullptr;
        default:
            return parseExpression();
    }
    throw std::runtime_error("Could not parse function level statements");
}


FunctionDeclaration*
Parser::parseFunctionDeclaration()
{
    auto functionDeclaration = createDeclaration<FunctionDeclaration>(SyntaxKind::FunctionDeclaration);
    functionDeclaration->routine = nullptr;
    functionDeclaration->name = parseName();
    Token token = takeNextToken();
    if (token != Token::OpenParen)
    {
        throw ExpectedTokenError(Token::OpenParen, getTokenValue());
    }
    auto parameterListResult = parseParameterListonOpenParen();
    functionDeclaration->parameterList = parameterListResult.parameterList;
    expectToken(Token::Colon);
    functionDeclaration->type = parseType();
    token = takeNextToken();
    if (token == Token::FromKeyword)
    {
        expectToken(Token::Identifier);
        functionDeclaration->from = parseName();
        token = takeNextToken();
    }
    if (isFunctionMetadataKeyword(token))
    {
//        functionDeclaration->metadata = parseFunctionMetadata();
        token = takeNextToken();
    }
    if (token == Token::SemiColon)
    {
        functionDeclaration->body = nullptr;
        goto finishDeclaration;
    }
    else if (token != Token::OpenBrace)
    {
        throw ExpectedTokenError(Token::OpenBrace, getTokenValue());
    }
    functionDeclaration->body = parseFunctionBodyOnOpenBrace();

    finishDeclaration:
    finishDeclaration(functionDeclaration);
    functionDeclaration->domain = _currentDomain;
    return functionDeclaration;
}


bool
Parser::isFunctionMetadataKeyword(Token token)
{
    switch (token)
    {
        case Token::ContextKeyword:
        case Token::AsyncKeyword:
        case Token::ParamsKeyword:
        case Token::TypesKeyword:
        case Token::PublicKeyword:
            return true;
        default:
            return false;
    }
}


//FunctionMetadata*
//Parser::parseFunctionMetadata()
//{
//    auto metadata = createSyntax<FunctionMetadata>(SyntaxKind::FunctionMetadata);
//    while (true)
//    {
//        Token token = peekNextToken();
//        switch (token)
//        {
//            case Token::OpenBrace:
//                goto outer;
//        }
//        takeNextToken();
//    }
//    outer:
//    return metadata;
//}


VariableDeclaration*
Parser::parseVariableDeclaration()
{
    VariableDeclaration* variableDeclaration = createDeclaration<VariableDeclaration>(SyntaxKind::VariableDeclaration);
    variableDeclaration->name = parseName();
    Token peek = peekNextToken();
    if (peek == Token::Colon)
    {
        skipNextToken();
        variableDeclaration->type = parseType();
        peek = peekNextToken();
    }
    if (peek == Token::Equal)
    {
        skipNextToken();
        variableDeclaration->expression = parseExpression();
        expectToken(Token::SemiColon);
    }
    return variableDeclaration;
}


Name*
Parser::parseName()
{
    expectToken(Token::Identifier);
    return createName();
}


ParameterListResult
Parser::parseParameterList()
{
    expectToken(Token::OpenParen);
    return parseParameterListonOpenParen();
}


ParameterListResult
Parser::parseParameterListonOpenParen()
{
    ParameterDeclarationList* parameterList = createSyntax<ParameterDeclarationList>(SyntaxKind::ParameterDeclarationList);
    Token token = takeNextToken();
    Utf8String parameterListDisplay = "(";
    while (true)
    {
        if (token == Token::CloseParen)
        {
            break;
        }
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        ParameterDeclaration* parameter = parseParameterOnIdentifier();
        parameter->display = getParameterDisplay(parameter);
        parameterListDisplay += parameter->display;
        parameterList->parameters.add(parameter);
        token = takeNextToken();
        if (token == Token::Comma)
        {
            parameterListDisplay += ", ";
            token = takeNextToken();
            continue;
        }
    }
    parameterListDisplay += ")";
    finishSyntax(parameterList);
    return { parameterList, parameterListDisplay };
}

ParameterDeclaration*
Parser::parseParameterOnIdentifier()
{
    ParameterDeclaration* parameter = createSyntax<ParameterDeclaration>(SyntaxKind::ParameterDeclaration);
    parameter->name = createName();
    expectToken(Token::Colon);
    parameter->type = parseType();
    finishSyntax(parameter);
    return parameter;
}


StatementBlock*
Parser::parseStatementBlock()
{
    StatementBlock* body = createBlock<StatementBlock>(SyntaxKind::FunctionBody);
    List<Syntax*> statements;
    while (peekNextToken() != Token::CloseBrace)
    {
        Syntax* statement = parseFunctionLevelStatement();
        if (!statement)
        {
            break;
        }
        statements.add(statement);
    }
    body->statements = statements;
    expectToken(Token::CloseBrace);
    return body;
}


StatementBlock*
Parser::parseFunctionBodyOnOpenBrace()
{
    return parseStatementBlock();
}


template<typename T>
T*
Parser::createSyntax(SyntaxKind kind)
{
    T* syntax = new T();
    syntax->kind = kind;
    syntax->start = _scanner->getStartPosition();
    return syntax;
}


template<typename T>
T*
Parser::createDeclaration(const SyntaxKind kind)
{
    auto syntax = createSyntax<T>(kind);
    syntax->labels = LABEL__DECLARATION;
    syntax->offset = 0;
    syntax->domain = _currentDomain;
    return syntax;
}


template<typename T>
T*
Parser::createTypeDeclaration(SyntaxKind kind)
{
    T* syntax = createSyntax<T>(kind);
    syntax->labels = LABEL__TYPE_DECLARATION;
    syntax->offset = 0;
    return syntax;
}


template<typename T>
T*
Parser::createBlock(SyntaxKind kind)
{
    T* syntax = createSyntax<T>(kind);
    syntax->labels = LABEL__BLOCK;
    return syntax;
}


template<typename T>
T*
Parser::createNamedExpression(SyntaxKind kind)
{
    T* syntax = createSyntax<T>(kind);
    syntax->labels = LABEL__NAMED_EXPRESSION;
    syntax->name = createName();
    return syntax;
}


template<typename T>
void
Parser::finishSyntax(T* syntax)
{
    syntax->end = _scanner->getPosition();
}


Utf8StringView
Parser::getTokenValue()
{
    return _scanner->getTokenValue();
}


void
Parser::expectToken(Token expected)
{
    Token result = _scanner->takeNextToken();
    assertToken(result, expected);
}

void
Parser::assertToken(Token target, Token expected)
{
    if (target != expected)
    {
        throw ExpectedTokenError(target, getTokenValue());
    }
}


Token
Parser::peekNextToken()
{
    return _scanner->peekNextToken();
}


Token
Parser::takeNextToken()
{
    return _scanner->takeNextToken();
}


TypeAssignment*
Parser::parseType()
{
    TypeAssignment* typeAssignment = createSyntax<TypeAssignment>(SyntaxKind::Type);
    Token token = _scanner->takeNextToken();
    switch (token) {
        case Token::IntKeyword:
            typeAssignment->type = TypeKind::Int;
            break;
        case Token::UnsignedIntKeyword:
            typeAssignment->type = TypeKind::UInt;
            break;
        case Token::CharKeyword:
            typeAssignment->type = TypeKind::Char;
            break;
        case Token::VoidKeyword:
            typeAssignment->type = TypeKind::Void;
            break;
        case Token::USizeKeyword:
            typeAssignment->type = TypeKind::USize;
            break;
        case Token::LengthOfKeyword:
            typeAssignment->type = TypeKind::Length;
            typeAssignment->size = 0;
            typeAssignment->name = createName();
            typeAssignment->parameter = parseName();
            return typeAssignment;
        case Token::StringKeyword:
        case Token::Identifier:
            typeAssignment->type = TypeKind::Custom;
            break;
        default:
            throw UnknownTypeError();
    }
    typeAssignment->size = 0; // Size is set in the transformer.
    typeAssignment->name = createName();
    while (true)
    {
        Token peek = peekNextToken();
        if (peek == Token::Asterisk)
        {
            if (typeAssignment->isLiteral)
            {
                throw ExpectedTokenError(Token::CloseParen, getTokenValue());
            }
            skipNextToken();
            typeAssignment->pointers.add(createPunctuation(PunctuationType::Asterisk));
        }
        else if (peek == Token::LiteralKeyword)
        {
            typeAssignment->isLiteral = true;
            skipNextToken();
        }
        else if (peek == Token::Ampersand)
        {
//            typeAssignment->addressOf = true;
//            skipNextToken();
        }
        else
        {
            break;
        }
    }
    return typeAssignment;
}


Name*
Parser::createName()
{
    Name* name = createSyntax<Name>(SyntaxKind::Name);
    name->name = getTokenValue();
    finishSyntax(name);
    return name;
}


CallExpression*
Parser::parseCallExpressionOnName()
{
    CallExpression* expression = createNamedExpression<CallExpression>(SyntaxKind::CallExpression);
    expectToken(Token::OpenParen);
    expression->argumentList = parseArgumentListOnOpenParen();
    finishSyntax(expression);
    expectToken(Token::SemiColon);
    return expression;
}


ArgumentList*
Parser::parseArgumentListOnOpenParen()
{
    ArgumentList* argumentList = createSyntax<ArgumentList>(SyntaxKind::ArgumentList);
    Token token = takeNextToken();
    while (true)
    {
        if (token == Token::CloseParen)
        {
            break;
        }
        Expression* argument = parseExpressionOnToken(token);
        argumentList->arguments.add(argument);
        token = takeNextToken();
        if (token == Token::Comma)
        {
            token = takeNextToken();
            continue;
        }
    }
    finishSyntax(argumentList);
    return argumentList;
}


Expression*
Parser::parseExpression()
{
    Token token = takeNextToken();
    Expression* expr = parseExpressionOnToken(token);
    Token peek = peekNextToken();

    // Given binary expressions A [*] B [*] C:
    // * Parse A and B and then check if after B has a binary operator.
    //   * If B has higher precedence:
    //     * B becomes left and then continue parseRightHandSideOfBinaryExpression(TokenPrecedence lowestPrecedence).
    //        * Where parseRightHandSideOfBinaryExpression only continues parsing expressions in the right-hand side
    //          if it encounters higher token precedence. For instance, if it encounters A [*:1] B [*:2] C [*:3] D,
    //          where ':n' denotes the precedence. From [*:2], parseRightHandSideOfBinaryExpression must evaluate [*:3]
    //          It will evaluate whether [*:3] has a higher precedence then the passed in lowestPrecedence [*:2].
    //
    //   * If B has lower precedence:
    //     * Create a binary expression of A and B first and then with C.
    if (isBinaryOperator(peek))
    {
        Expression* leftExpression = expr;
        while (isBinaryOperator(peek))
        {
            BinaryExpression* binaryExpression = createSyntax<BinaryExpression>(SyntaxKind::BinaryExpression);
            unsigned int precedence = getOperatorPrecedence(peek);
            takeNextToken();
            binaryExpression->left = leftExpression;
            binaryExpression->binaryOperatorKind = getBinaryOperatorKind(peek);
            binaryExpression->right = parseRightHandSideOfBinaryExpression(precedence);
            leftExpression = binaryExpression;
            peek = peekNextToken();
        }
        return leftExpression;
    }
    return expr;
}


Expression*
Parser::parseRightHandSideOfBinaryExpression(unsigned int previousOperatorPrecedence)
{
    Token token = takeNextToken();
    Expression* expression = parseExpressionOnToken(token);
    Token peek = peekNextToken();
    if (isBinaryOperator(peek))
    {
        unsigned int nextOperatorPrecedence = getOperatorPrecedence(peek);
        if (nextOperatorPrecedence > previousOperatorPrecedence)
        {
            BinaryExpression* binaryExpression = createSyntax<BinaryExpression>(SyntaxKind::BinaryExpression);
            binaryExpression->left = expression;
            binaryExpression->binaryOperatorKind = getBinaryOperatorKind(peek);
            binaryExpression->right = parseRightHandSideOfBinaryExpression(nextOperatorPrecedence);
            return binaryExpression;
        }
    }
    return expression;
}


Expression*
Parser::parseExpressionOnToken(Token token)
{
    switch (token)
    {
        case Token::Ampersand:
            return parseAddressOfExpression();
        case Token::StringLiteral:
        {
            StringLiteral *stringLiteral = createSyntax<StringLiteral>(SyntaxKind::BooleanLiteral);
            finishSyntax(stringLiteral);
            stringLiteral->stringStart = stringLiteral->start + 1;
            stringLiteral->stringEnd = stringLiteral->end - 1;
            return stringLiteral;
        }
        case Token::IntegerLiteral:
        {
            IntegerLiteral* integer = createSyntax<IntegerLiteral>(SyntaxKind::IntegerLiteral);
            integer->value = getInteger(integer);
            return integer;
        }
        case Token::TrueKeyword:
            return createBooleanLiteral(true);
        case Token::FalseKeyword:
            return createBooleanLiteral(false);
        case Token::OpenBracket:
            return parseArrayLiteral();
        case Token::OpenParen:
            return parseTuple();
        case Token::LengthOfKeyword:
            return parseLengthOfExpression();
        case Token::Identifier:
            return parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();
    }
    throw UnexpectedExpression();
}

BooleanLiteral*
Parser::createBooleanLiteral(bool value)
{
    BooleanLiteral* boolean = createSyntax<BooleanLiteral>(SyntaxKind::BooleanLiteral);
    boolean->value = true;
    return boolean;
}


Tuple*
Parser::parseTuple()
{
    Tuple* tuple = createSyntax<Tuple>(SyntaxKind::Tuple);
    Token peek = peekNextToken();
    while (true)
    {
        if (peek == Token::CloseParen)
        {
            skipNextToken();
            break;
        }

        Expression* expression = parseExpression();
        tuple->values.add(expression);
        peek = peekNextToken();
        if (peek == Token::Comma)
        {
            skipNextToken();
            peek = peekNextToken();
        }
    }
    finishSyntax(tuple);
    return tuple;
}


UsingStatement*
Parser::parseUsingStatement()
{
    UsingStatement* usingStatement = createSyntax<UsingStatement>(SyntaxKind::UsingStatement);
    usingStatement->labels |= LABEL__USING_STATEMENT;
    usingStatement->domain = _currentDomain;
    expectToken(Token::Identifier);
    usingStatement->domains.add(createName());
    expectToken(Token::DoubleColon);
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::Identifier)
        {
            usingStatement->domains.add(createName());
            expectToken(Token::DoubleColon);
            continue;
        }
        else if (token == Token::OpenBrace)
        {
            usingStatement->usageClause = parseUsageClauseOnOpenBrace();
            break;
        }
        else
        {
            throw ExpectedTokenError(Token::OpenBrace, getTokenValue());
        }
    }
    finishSyntax(usingStatement);
    return usingStatement;
}


Punctuation*
Parser::createPunctuation(PunctuationType type)
{
    Punctuation* punctuation = createSyntax<Punctuation>(SyntaxKind::Punctuation);
    punctuation->type = type;
    finishSyntax(punctuation);
    return punctuation;
}


Expression*
Parser::parsePropertyAccessOrCallExpression()
{
    expectToken(Token::Identifier);
    Token peek = peekNextToken();
    return createPropertyAccessExpressionOrCallExpressionFromPeek(peek);
}


Expression*
Parser::createPropertyAccessExpressionOrCallExpressionFromPeek(Token peek)
{
    if (peek == Token::Dot)
    {
        auto propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
        skipNextToken();
        propertyAccessExpression->expression = parsePropertyAccessOrCallExpression();
        finishSyntax(propertyAccessExpression);
        return propertyAccessExpression;
    }
    if (peek == Token::OpenParen)
    {
        return parseCallExpressionOnName();
    }
    auto propertyExpression = createSyntax<PropertyExpression>(SyntaxKind::PropertyExpression);
    propertyExpression->labels |= LABEL__NAMED_EXPRESSION;
    propertyExpression->name = createName();
    finishSyntax(propertyExpression);
    return propertyExpression;
}


void
Parser::skipNextToken()
{
    _scanner->takeNextToken();
}


UsageClause*
Parser::parseUsageClauseOnOpenBrace()
{
    UsageClause* usageClause = createSyntax<UsageClause>(SyntaxKind::NamedUsage);
    bool isInitialUsage = true;
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::Comma)
        {
            if (isInitialUsage)
            {
                throw ExpectedTokenError(Token::Identifier, getTokenValue());
            }
            token = takeNextToken();
        }
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        usageClause->names.add(createName());
        isInitialUsage = false;
    }
    finishSyntax(usageClause);
    expectToken(Token::SemiColon);
    return usageClause;
}


ClassDeclaration*
Parser::parseObjectDeclaration()
{
    ClassDeclaration* objectDeclaration = createDeclaration<ClassDeclaration>(SyntaxKind::ObjectDeclaration);
    _scanner->setFlag(TREAT_STRING_KEYWORD_AS_NAME);
    Token token = takeNextToken();
    if (token == Token::Identifier || token == Token::StringKeyword)
    {
        objectDeclaration->name = createName();
    }
    else
    {
        throw ExpectedTokenError(Token::Identifier, getTokenValue());
    }
    expectToken(Token::OpenBrace);
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        Name* name = createName();
        if (objectDeclaration->kind == SyntaxKind::ObjectDeclaration && name->name == objectDeclaration->name->name)
        {
            objectDeclaration->constructor = parseConstructorDeclaration();
            expectToken(Token::SemiColon);
        }
        else
        {
            objectDeclaration->properties.add(parsePropertyDeclarationOnIdentifier(name));
            expectToken(Token::SemiColon);
        }
    }
    finishDeclaration(objectDeclaration);
    _scanner->resetFlags();
    return objectDeclaration;
}


InterfaceDeclaration*
Parser::parseInterfaceDeclaration()
{
    auto interface = createDeclaration<InterfaceDeclaration>(SyntaxKind::InterfaceDeclaration);
    interface->name = parseName();
    Token token = takeNextToken();
//    if (token == Token::PublicKeyword)
//    {
//        interface->publicKeyword = createSyntax<Modifier>(SyntaxKind::Modifier);
//        finishSyntax(interface->publicKeyword);
//        token = takeNextToken();
//    }
//    else
//    {
//        interface->publicKeyword = nullptr;
//    }
    assertToken(token, Token::OpenBrace);
    while (true)
    {
        token = takeNextToken();
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        interface->properties.add(parsePropertyDeclarationOnIdentifier(nullptr));
    }
    finishTypeDeclaration(interface);
    return interface;
}


ConstructorDeclaration*
Parser::parseConstructorDeclaration()
{
    auto constructor = createSyntax<ConstructorDeclaration>(SyntaxKind::ConstructorDeclaration);
    auto result = parseParameterList();;
    constructor->parameterList = result.parameterList;
    finishSyntax(constructor);
    return constructor;
}


PropertyDeclaration*
Parser::parsePropertyDeclarationOnIdentifier(Name* name)
{
    if (name == nullptr)
    {
        name = createName();
    }
    PropertyDeclaration* property = createSyntax<PropertyDeclaration>(SyntaxKind::PropertyDeclaration);
    property->name = name;
    Token peek = peekNextToken();
    if (peek == Token::OpenParen)
    {
        auto result = parseParameterList();;
        property->parameters = result.parameterList;
    }
    expectToken(Token::Colon);
    property->type = parseType();
    Token token = takeNextToken();
    if (token == Token::StaticKeyword)
    {
        property->isStatic = true;
        expectToken(Token::SemiColon);
    }
    else
    {
        assertToken(token, Token::SemiColon);
    }
    finishSyntax(property);
    return property;
}


EnumDeclaration*
Parser::parseEnumDeclaration()
{
    EnumDeclaration* enumDeclaration = createDeclaration<EnumDeclaration>(SyntaxKind::EnumDeclaration);
    enumDeclaration->name = parseName();
    expectToken(Token::OpenBrace);
    while (true)
    {
        Token token = takeNextToken();
        if (token == Scanner::Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        enumDeclaration->values.add(createName());
        expectToken(Token::Comma);
    }
    finishSyntax(enumDeclaration);
    return enumDeclaration;
}


AssemblyBlock*
Parser::parseAssemblyBlock()
{
    AssemblyBlock* assemblyBlock = createSyntax<AssemblyBlock>(SyntaxKind::AssemblyBlock);
    expectToken(Token::Colon);
    assemblyBlock->metadata = parseDeclarationMetadata();
    assemblyBlock->body = parseAssemblyBody();
    finishSyntax(assemblyBlock);
    return assemblyBlock;
}



DomainDeclaration*
Parser::parseDomainDeclaration()
{
    _currentDomain = createDeclaration<DomainDeclaration>(SyntaxKind::DomainDeclaration);
    _currentDomain->implementsClause = nullptr;
    Token token = takeNextToken();
    char* start = _scanner->getStartPosition();
    while (true)
    {
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        _currentDomain->names.add(createName());
        char *end = _scanner->getPosition();
        token = takeNextToken();
        if (token == Token::SemiColon)
        {
            auto name = createSyntax<Name>(SyntaxKind::Name);
            name->start = start;
            name->end = end;
            name->name = Utf8StringView(start, end);
            _currentDomain->name = name;
            addSymbol(_currentDomain);
            finishSyntax(_currentDomain);
            _currentDomain->block = nullptr;
            return _currentDomain;
        }
        if (token == Token::ImplementsKeyword)
        {
            _currentDomain->implementsClause = parseName();
            expectToken(Token::OpenBrace);
            _currentDomain->block = parseDeclarationBlock();
            goto setAccessMap;
        }
        if (token == Token::OpenBrace)
        {
            _currentDomain->block = parseDeclarationBlock();
            goto setAccessMap;
        }
        if (token != Token::DoubleColon)
        {
            throw ExpectedTokenError(Token::DoubleColon, getTokenValue());
        }
        token = takeNextToken();
    }

    setAccessMap:
    std::size_t last = _currentDomain->names.size() - 1;
    ast::DomainDeclarationMap* currentAccessMap = &domainDeclarationMap;
    for (int i = 0; i <= last; i++)
    {
        auto name = _currentDomain->names[i];
        if (i == last)
        {
            currentAccessMap->insert({ name->name, &_currentDomain->block->symbols });
            break;
        }
        ast::DomainDeclarationMap* newAccessMap = new ast::DomainDeclarationMap();
        currentAccessMap->insert({ name->name, newAccessMap });
        currentAccessMap = newAccessMap;
    }
    return _currentDomain;
}


DeclarationMetadata*
Parser::parseDeclarationMetadata()
{
    DeclarationMetadata* metadata = createSyntax<DeclarationMetadata>(SyntaxKind::DeclarationMetadata);
    Token token = takeNextToken();
    while (true)
    {
        if (token == Token::OpenBrace)
        {
            break;
        }
        if (token != Token::Identifier)
        {
            throw ExpectedTokenError(Token::Identifier, getTokenValue());
        }
        MetadataProperty* property = createSyntax<MetadataProperty>(SyntaxKind::MetadataProperty);
        property->name = createName();
        expectToken(Token::Colon);
        property->value = parseExpression();
        finishSyntax(property);
        metadata->properties.add(property);
        token = takeNextToken();
        if (token == Token::Comma)
        {
            token = takeNextToken();
        }
    }
    finishSyntax(metadata);
    return metadata;
}


Expression*
Parser::parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier()
{
    Token peek = peekNextToken();
    if (peek == Token::DoubleColon)
    {
        ModuleAccessExpression* moduleAccessExpression = createSyntax<ModuleAccessExpression>(SyntaxKind::ModuleAccessExpression);
        moduleAccessExpression->name = createName();
        skipNextToken();
        expectToken(Token::Identifier);
        moduleAccessExpression->expression = parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();
        finishSyntax(moduleAccessExpression);
        return moduleAccessExpression;
    }
    else
    {
        return createPropertyAccessExpressionOrCallExpressionFromPeek(peek);
    }
}


ArrayLiteral*
Parser::parseArrayLiteral()
{
    auto arrayLiteral = createSyntax<ArrayLiteral>(SyntaxKind::ArrayLiteral);
    Token peek = peekNextToken();
    while (true)
    {
        if (peek == Token::CloseBracket)
        {
            skipNextToken();
            break;
        }
        Expression* expression = parseExpression();
        arrayLiteral->values.add(expression);
        peek = peekNextToken();
        if (peek == Token::Comma)
        {
            skipNextToken();
            peek = peekNextToken();
        }
    }
    finishSyntax(arrayLiteral);
    return arrayLiteral;
}

AssemblyBody*
Parser::parseAssemblyBody()
{
    AssemblyBody* body = createSyntax<AssemblyBody>(SyntaxKind::AssemblyBody);
    if (!_instructionParser)
    {
        _instructionParser = new InstructionParser(_scanner->getSource());
    }
    _instructionParser->seek(_scanner->getLocation());
    List<output::Instruction*>* instructions = _instructionParser->parse();
    body->instructions = instructions;
    seek(_instructionParser->getLocation());
    expectToken(Token::CloseBrace);
    finishSyntax(body);
    return body;
}


void
Parser::seek(const BaseScanner::Location& location)
{
    _scanner->seek(location);
}


LengthOfExpression*
Parser::parseLengthOfExpression()
{
    LengthOfExpression* expression = createSyntax<LengthOfExpression>(SyntaxKind::LengthOfExpression);
    expression->reference = parseName();
    return expression;
}


Utf8String
Parser::getParameterDisplay(ParameterDeclaration* parameter)
{
    Utf8String label = parameter->type->name->name.toString();
    for (unsigned int i = 0; i < parameter->type->pointers.size(); i++)
    {
        label += "*";
    }
    return label;
}


void
Parser::addSymbol(Declaration* declaration)
{
    auto symbol = new Symbol(_symbolOffset, declaration->name->name);
    symbols->add(symbol);
    declaration->symbol = symbol;
    if (_isInCBlock)
    {
        declaration->symbol->externalSymbol = Utf8String("_") + symbol->name.toString();
    }
    else
    {
        declaration->symbol->externalSymbol = symbol->name.toString();
    }
}


void
Parser::addTypeSymbol(Declaration* declaration)
{
    auto symbol = new Symbol(_symbolOffset, declaration->name->name);
    symbols->add(symbol);
    declaration->symbol = symbol;
}


template<typename T>
void
Parser::finishDeclaration(T* declaration)
{
    addSymbol(declaration);
    finishSyntax(declaration);
}


template<typename T>
void
Parser::finishTypeDeclaration(T* declaration)
{
    addTypeSymbol(declaration);
    finishSyntax(declaration);
}


AddressOfExpression*
Parser::parseAddressOfExpression()
{
    auto addressOfExpression = createSyntax<AddressOfExpression>(SyntaxKind::AddressOfExpression);
    addressOfExpression->expression = parseExpression();
    finishSyntax(addressOfExpression);
    return addressOfExpression;
}


ExternCBlock*
Parser::parseExternCBlock()
{
    _isInCBlock = true;
    ExternCBlock* block = createSyntax<ExternCBlock>(SyntaxKind::ExternCBlock);
    expectToken(Token::StringLiteral);
    if (getTokenValue() != "\"C\"")
    {
        throw OnlyExternCBlocksAllowedError();
    }
    expectToken(Token::OpenBrace);
    block->declarations = parseExternCBlockLevelDeclarations();
    _isInCBlock = false;
    return block;
}


List<Declaration*>
Parser::parseExternCBlockLevelDeclarations()
{
    List<Declaration*> list;
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::FunctionKeyword)
        {
            Declaration* declaration = parseFunctionDeclaration();
            list.add(declaration);
        }
        else if (token == Token::CloseBrace)
        {
            break;
        }
        else if (token == Token::EndOfFile)
        {
            throw UnexpectedEndOfFile();
        }
    }
    return list;
}


IfStatement*
Parser::parseIfStatement()
{
    IfStatement* ifStatement = createSyntax<IfStatement>(SyntaxKind::IfStatement);
    expectToken(Token::OpenParen);
    ifStatement->condition = parseExpression();
    expectToken(Token::CloseParen);
    expectToken(Token::OpenBrace);
    ifStatement->body = parseStatementBlock();
    return ifStatement;
}


BinaryOperatorKind
Parser::getBinaryOperatorKind(Token token)
{
    switch (token)
    {
        case Token::AmpersandAmpersand:
            return BinaryOperatorKind::And;
        case Token::PipePipe:
            return BinaryOperatorKind::Or;
        case Token::EqualEqual:
            return BinaryOperatorKind::Equal;
        case Token::Plus:
            return BinaryOperatorKind::Plus;
        case Token::Minus:
            return BinaryOperatorKind::Minus;
        default:
            throw std::runtime_error("Unknown binary operator.");
    }
}

ReturnStatement*
Parser::parseReturnStatement()
{
    ReturnStatement* returnStatement = createSyntax<ReturnStatement>(SyntaxKind::ReturnStatement);
    returnStatement->expression = parseExpression();
    expectToken(Token::SemiColon);
    return returnStatement;
}


unsigned int
Parser::getOperatorPrecedence(Token token) const
{
    switch (token)
    {
        case Token::Minus:
        case Token::Plus:
            return 1;
        default:
            throw std::runtime_error("Unknown operator token.");
    }
}


bool
Parser::isBinaryOperator(Token token) const
{
    return token >= Token::BinaryOperationStart && token <= Token::BinaryOperationEnd;
}


unsigned int
Parser::getInteger(IntegerLiteral* integerLiteral)
{
    const char* cursor = integerLiteral->end;
    unsigned int exponent = 0;
    unsigned int result;
    while (cursor != integerLiteral->start)
    {
        unsigned int s = cursor[0] - '0';
        result += std::pow(s, 10);
        cursor--;
    }
    return result;
}


}