#include <Foundation/Path.h>
#include "Parser.h"
#include "Parser.Error.h"
#include "Exceptions.h"

using Token = Scanner::Token;

thread_local
Path*
Parser::_currentDirectory = nullptr;

Parser::Parser(Compiler* compiler):
    _compiler(compiler),
    _scanner(nullptr)
{

}


List<Syntax*>
Parser::parse(const char* sourceStart, const char* sourceEnd, const Path* currentDirectory)
{
    Utf8StringView sourceString(sourceStart, sourceEnd);
    Scanner scanner(sourceString);
    _currentDirectory = const_cast<Path*>(currentDirectory);
    _scanner = &scanner;
    List<Syntax*> statements;
    while (true)
    {
        Syntax* statement = parseStatement();
        if (!statement)
        {
            break;
        }
        statements.add(statement);
    }
    return statements;
}


Syntax*
Parser::parseStatement()
{
    Token token = _scanner->takeNextToken();
    switch (token)
    {
        case Token::InterfaceKeyword:
            return parseInterfaceDeclaration();
        case Token::EnumKeyword:
            return parseEnumDeclaration();
        case Token::EndKeyword:
            return parseEndStatement();
        case Token::ModuleKeyword:
            return parseModuleDeclaration();
        case Token::ImportKeyword:
            return parseImportStatement();
        case Token::ExportKeyword:
            return parseExportStatement();
        case Token::UseKeyword:
            return parseUseStatement();
        case Token::FunctionKeyword:
            return parseFunctionDeclaration();
        case Token::Identifier:
        {
            Token peek = peekNextToken();
            switch (peek)
            {
                case Token::OpenParen:
                    return parseCallExpressionOnIdentifier();
                case Token::Colon:
                    return nullptr;
                case Token::Dot:
                    PropertyAccessExpression* propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
                    propertyAccessExpression->name = createIdentifer();
                    skipNextToken();
                    propertyAccessExpression->dot = createPunctuation(PunctuationType::Dot);
                    propertyAccessExpression->expression = parsePropertyAccessOrCallExpression();
                    return propertyAccessExpression;
            }
        }
        case Token::EndOfFile:
            return nullptr;
        default:
            return parseExpression();
    }
}


FunctionDeclaration*
Parser::parseFunctionDeclaration()
{
    FunctionDeclaration* _function = createSyntax<FunctionDeclaration>(SyntaxKind::FunctionDeclaration);
    _function->name = parseIdentifier();
    _function->parameterList = parseParameterList();
    expectToken(Token::Colon);
    _function->type = parseType();
    _function->body = parseFunctionBody();
    finishSyntax<FunctionDeclaration>(_function);
    return _function;
}

Identifier*
Parser::parseIdentifier()
{
    expectToken(Token::Identifier);
    return createIdentifer();
}

ParameterList*
Parser::parseParameterList()
{
    ParameterList* parameterList = createSyntax<ParameterList>(SyntaxKind::ParameterList);
    expectToken(Token::OpenParen);
    bool isInitialParameter = true;
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::CloseParen)
        {
            break;
        }
        if (token == Token::Comma)
        {
            if (isInitialParameter)
            {
                throw ExpectedTokenError(Token::Identifier, getTokenValue());
            }
            expectToken(Token::Identifier);
        }
        Parameter* parameter = parseParameterOnIdentifier();
        parameterList->parameters.add(parameter);
        isInitialParameter = false;
    }
    finishSyntax(parameterList);
    return parameterList;
}

Parameter*
Parser::parseParameterOnIdentifier()
{
    Parameter* parameter = createSyntax<Parameter>(SyntaxKind::Parameter);
    parameter->name = createIdentifer();
    expectToken(Token::Colon);
    parameter->type = parseType();
    finishSyntax(parameter);
    return parameter;
}

FunctionBody*
Parser::parseFunctionBody()
{
    expectToken(Token::OpenBrace);
    FunctionBody* body = createSyntax<FunctionBody>(SyntaxKind::FunctionBody);
    List<Syntax*> statements;
    while (peekNextToken() != Token::CloseBrace)
    {
        Syntax* statement = parseStatement();
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
Parser::expectToken(Token token)
{
    Token result = _scanner->takeNextToken();
    if (result != token)
    {
        throw ExpectedTokenError(token, getTokenValue());
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


Type*
Parser::parseType()
{
    Type* type = createSyntax<Type>(SyntaxKind::Unknown);
    Token token = _scanner->takeNextToken();
    switch (token) {
        case Token::IntKeyword:
            type->type = TypeKind::Int;
            break;
        case Token::UnsignedIntKeyword:
            type->type = TypeKind::UnsignedInt;
            break;
        case Token::VoidKeyword:
            type->type = TypeKind::Void;
            break;
        case Token::Identifier:
            type->type = TypeKind::Custom;
            break;
        default:
            throw UnknownTypeError();
    }
    type->name = createIdentifer();
    while (true)
    {
        Token peek = peekNextToken();
        if (peek == Token::Asterisk)
        {
            skipNextToken();
            type->pointer.add(createPunctuation(PunctuationType::Asterisk));
        }
        else
        {
            break;
        }
    }
}


Identifier*
Parser::createIdentifer()
{
    Identifier* identifier = createSyntax<Identifier>(SyntaxKind::Identifier);
    identifier->name = getTokenValue();
    finishSyntax(identifier);
    return identifier;
}


CallExpression*
Parser::parseCallExpressionOnIdentifier()
{
    CallExpression* expression = createSyntax<CallExpression>(SyntaxKind::CallExpression);
    expression->name = createIdentifer();
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
    while (peekNextToken() != Token::EndOfFile)
    {
        Expression* argument = parseExpression();
        argumentList->arguments.add(argument);
        Token peek = peekNextToken();
        if (peek == Token::Comma)
        {
            skipNextToken();
            continue;
        }
        else if (peek == Token::CloseParen)
        {
            skipNextToken();
            break;
        }
        else
        {
            throw ExpectedTokenError(Token::Comma, getTokenValue());
        }
    }
    finishSyntax(argumentList);
    return argumentList;
}

Expression*
Parser::parseExpression()
{
    Token token = takeNextToken();
    switch (token)
    {
        case Token::StringLiteral:
        {
            StringLiteral *stringLiteral = createSyntax<StringLiteral>(SyntaxKind::StringLiteral);
            finishSyntax(stringLiteral);
            return stringLiteral;
        }
    }
    return nullptr;
}

UseStatement*
Parser::parseUseStatement()
{
    UseStatement* useStatement = createSyntax<UseStatement>(SyntaxKind::UseStatement);
    expectToken(Token::Identifier);
    useStatement->usage = parseModuleAccessUsageOnIdentifier();
    finishSyntax(useStatement);
    return useStatement;
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
    if (peek == Token::Dot)
    {
        PropertyAccessExpression* propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
        skipNextToken();
        propertyAccessExpression->expression = parsePropertyAccessOrCallExpression();
        finishSyntax(propertyAccessExpression);
        return propertyAccessExpression;
    }
    else if (peek == Token::OpenParen)
    {
        return parseCallExpressionOnIdentifier();
    }
    else
    {
        PropertyExpression* propertyExpression = createSyntax<PropertyExpression>(SyntaxKind::PropertyExpression);
        propertyExpression->name = createIdentifer();
        finishSyntax(propertyExpression);
        return propertyExpression;
    }
}

void
Parser::skipNextToken()
{
    _scanner->takeNextToken();
}


ImportStatement*
Parser::parseImportStatement()
{
    ImportStatement* importStatement = createSyntax<ImportStatement>(SyntaxKind::ImportStatement);
    expectToken(Token::StringLiteral);
    StringLiteral* stringLiteral = createSyntax<StringLiteral>(SyntaxKind::StringLiteral);
    finishSyntax(stringLiteral);
    Utf8StringView source(stringLiteral->start + 1, stringLiteral->end - 1);
    Path* newCurrenctDirectory = new Path(_currentDirectory->toString().toString());
    newCurrenctDirectory->add(source.toString());
    _compiler->parseFile(newCurrenctDirectory->toString().asString());
    importStatement->path = stringLiteral;
    finishSyntax(stringLiteral);
    finishSyntax(importStatement);
    expectToken(Token::SemiColon);
    return importStatement;
}


ExportStatement*
Parser::parseExportStatement()
{
    ExportStatement* exportStatement = createSyntax<ExportStatement>(SyntaxKind::ImportStatement);
    expectToken(Token::Identifier);
    exportStatement->usage = parseModuleAccessUsageOnIdentifier();
    return exportStatement;
}


ModuleDeclaration*
Parser::parseModuleDeclaration()
{
    ModuleDeclaration* module = createSyntax<ModuleDeclaration>(SyntaxKind::ModuleDeclaration);
    module->name = parseIdentifier();
    expectToken(Token::Colon);
    while(true)
    {
        Syntax* statement = parseStatement();
        if (!statement)
        {
            throw UnexpectedEndOfModule();
        }
        if (statement->kind == SyntaxKind::EndStatement)
        {
            EndStatement* endStatement = reinterpret_cast<EndStatement*>(statement);
            if (hasEqualIdentifier(endStatement->name, module->name))
            {
                module->endStatement = endStatement;
                finishSyntax(module);
                break;
            }
        }
        else
        {
            module->declarations.add(statement);
        }
    }
    return module;
}

bool
Parser::hasEqualIdentifier(Identifier *id1, Identifier* id2)
{
    return id1->name == id2->name;
}

EndStatement*
Parser::parseEndStatement()
{
    EndStatement* endStatement = createSyntax<EndStatement>(SyntaxKind::EndStatement);
    endStatement->name = parseIdentifier();
    finishSyntax(endStatement);
    expectToken(Token::SemiColon);
    return endStatement;
}

ModuleAccessUsage*
Parser::parseModuleAccessUsageOnIdentifier()
{
    ModuleAccessUsage* moduleAccessUsage = createSyntax<ModuleAccessUsage>(SyntaxKind::ModuleAccessUsage);
    moduleAccessUsage->name = createIdentifer();
    expectToken(Token::DoubleColon);
    Token peek = peekNextToken();
    if (peek == Token::Identifier)
    {
        skipNextToken();
        moduleAccessUsage->usage = parseModuleAccessUsageOnIdentifier();
    }
    else if (peek == Token::OpenBrace)
    {
        skipNextToken();
        moduleAccessUsage->usage = parseNamedUsageOnOpenBrace();
    }
    else if (peek == Token::Asterisk)
    {
        skipNextToken();
        WildcardUsage* wildcardUsage = createSyntax<WildcardUsage>(SyntaxKind::WildcardUsage);
        finishSyntax(wildcardUsage);
        moduleAccessUsage->usage = wildcardUsage;
        expectToken(Token::SemiColon);
    }
    else
    {
        throw UnexpectedModuleAccessUsage();
    }
    finishSyntax(moduleAccessUsage);
    return moduleAccessUsage;
}

NamedUsage*
Parser::parseNamedUsageOnOpenBrace()
{
    NamedUsage* namedUsage = createSyntax<NamedUsage>(SyntaxKind::NamedUsage);
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
        namedUsage->names.add(createIdentifer());
        isInitialUsage = false;
    }
    finishSyntax(namedUsage);
    expectToken(Token::SemiColon);
    return namedUsage;
}

InterfaceDeclaration*
Parser::parseInterfaceDeclaration()
{
    InterfaceDeclaration* interface = createSyntax<InterfaceDeclaration>(SyntaxKind::InterfaceDeclaration);
    interface->name = parseIdentifier();
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
        Token peek = peekNextToken();
        PropertyDeclaration* property = createSyntax<PropertyDeclaration>(SyntaxKind::PropertyDeclaration);
        property->name = createIdentifer();
        if (peek == Token::OpenParen)
        {
            property->parameters = parseParameterList();
        }
        expectToken(Token::Colon);
        property->type = parseType();
        expectToken(Token::SemiColon);
        finishSyntax(property);
        interface->properties.add(property);
    }
    finishSyntax(interface);
    return interface;
}

EnumDeclaration*
Parser::parseEnumDeclaration()
{
    EnumDeclaration* enumDeclaration = createSyntax<EnumDeclaration>(SyntaxKind::EnumDeclaration);
    enumDeclaration->name = parseIdentifier();
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
        enumDeclaration->values.add(createIdentifer());
        expectToken(Token::Comma);
    }
    finishSyntax(enumDeclaration);
    return enumDeclaration;
}
