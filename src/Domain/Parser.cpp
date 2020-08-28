#include <Foundation/Path.h>
#include "Parser.h"
#include "Parser.Error.h"
#include "Exceptions.h"

namespace elet::domain::compiler::ast
{


thread_local
Path*
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
    _currentDirectory = const_cast<Path*>(task.sourceDirectory);
    _scanner = &scanner;
    List<Syntax*> statements;
    ParsingTask* pendingParsingTask = nullptr;
    while (true)
    {
        try
        {
            Syntax* statement = parseFileLevelStatement();
            if (!statement)
            {
                break;
            }
            if (statement->labels & DECLARATION_LABEL)
            {
                Declaration* declaration = reinterpret_cast<Declaration*>(statement);
                task.sourceFile->names.insert({ declaration->symbol->name, declaration });
                declaration->sourceFile = task.sourceFile;
            }
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


Syntax*
Parser::parseFileLevelStatement()
{
    Token token = _scanner->takeNextToken();
    switch (token)
    {
        case Token::ContextKeyword:
            return parseContextDeclaration();
        case Token::DomainKeyword:
            return parseDomainDeclaration();
        case Token::ConstructorKeyword:
            return parseConstructorImplementation();
        case Token::ObjectKeyword:
            return parseObjectDeclaration();
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
        case Token::EndOfFile:
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
        case Token::ObjectKeyword:
            return parseObjectDeclaration();
        case Token::InterfaceKeyword:
            return parseInterfaceDeclaration();
        case Token::EnumKeyword:
            return parseEnumDeclaration();
        case Token::EndKeyword:
            return parseEndStatement();
        case Token::ModuleKeyword:
            return parseModuleDeclaration();
        case Token::UseKeyword:
            return parseUseStatement();
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
    Syntax* statement;
    switch (token)
    {
        case Token::VarKeyword:
            statement = parseVariableDeclaration();
            break;
        case Token::AssemblyKeyword:
            return parseAssemblyBlock();
        case Token::Name:
        {
            Token peek = peekNextToken();
            switch (peek)
            {
                case Token::OpenParen:
                    statement = parseCallExpressionOnName();
                    goto outer;
                case Token::Colon:
                    return nullptr;
                case Token::Dot:
                {
                    PropertyAccessExpression *propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
                    propertyAccessExpression->name = createName();
                    skipNextToken();
                    propertyAccessExpression->dot = createPunctuation(PunctuationType::Dot);
                    propertyAccessExpression->expression = parsePropertyAccessOrCallExpression();
                    statement = propertyAccessExpression;
                }
            }
        }
        case Token::EndOfFile:
            return nullptr;
        default:
            statement = parseExpression();
    }
    outer:
    expectToken(Token::SemiColon);
    return statement;
}


FunctionDeclaration*
Parser::parseFunctionDeclaration()
{
    auto functionDeclaration = createDeclaration<FunctionDeclaration>(SyntaxKind::FunctionDeclaration);
    functionDeclaration->name = parseName();
    auto parameterListResult = parseParameterList();
    functionDeclaration->parameterList = parameterListResult.parameterList;
    expectToken(Token::Colon);
    functionDeclaration->type = parseType();
    functionDeclaration->body = parseFunctionBodyOnOpenBrace();
    addSymbol(functionDeclaration);
    finishSyntax<FunctionDeclaration>(functionDeclaration);
    return functionDeclaration;
}


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
    }
    return variableDeclaration;
}


Name*
Parser::parseName()
{
    expectToken(Token::Name);
    return createName();
}

ParameterListResult
Parser::parseParameterList()
{
    ParameterDeclarationList* parameterList = createSyntax<ParameterDeclarationList>(SyntaxKind::ParameterDeclarationList);
    expectToken(Token::OpenParen);
    Token token = takeNextToken();
    Utf8String parameterListDisplay = "(";
    while (true)
    {
        if (token == Token::CloseParen)
        {
            break;
        }
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
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


FunctionBody*
Parser::parseFunctionBody()
{
    FunctionBody* body = createBlock<FunctionBody>(SyntaxKind::FunctionBody);
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


FunctionBody*
Parser::parseFunctionBodyOnOpenBrace()
{
    expectToken(Token::OpenBrace);
    return parseFunctionBody();
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
Parser::createDeclaration(SyntaxKind kind)
{
    T* syntax = createSyntax<T>(kind);
    syntax->labels = DECLARATION_LABEL;
    syntax->offset = 0;
    return syntax;
}


template<typename T>
T*
Parser::createBlock(SyntaxKind kind)
{
    T* syntax = createSyntax<T>(kind);
    syntax->labels = BLOCK_LABEL;
    return syntax;
}


template<typename T>
T*
Parser::createNamedExpression(SyntaxKind kind)
{
    T* syntax = createSyntax<T>(kind);
    syntax->labels = NAMED_EXPRESSION_LABEL;
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
        case Token::Name:
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
            typeAssignment->addressOf = true;
            skipNextToken();
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
    return parseExpressionOnToken(token);
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
            StringLiteral *stringLiteral = createSyntax<StringLiteral>(SyntaxKind::StringLiteral);
            finishSyntax(stringLiteral);
            stringLiteral->stringStart = stringLiteral->start + 1;
            stringLiteral->stringEnd = stringLiteral->end - 1;
            return stringLiteral;
        }
        case Token::OpenBracket:
            return parseArrayLiteral();
        case Token::OpenParen:
            return parseTuple();
        case Token::LengthOfKeyword:
            return parseLengthOfExpression();
        case Token::Name:
            return parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();
    }
    throw UnexpectedExpression();
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


UseStatement*
Parser::parseUseStatement()
{
    UseStatement* useStatement = createSyntax<UseStatement>(SyntaxKind::UseStatement);
    expectToken(Token::Name);
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
    expectToken(Token::Name);
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
    propertyExpression->name = createName();
    finishSyntax(propertyExpression);
    return propertyExpression;
}


void
Parser::skipNextToken()
{
    _scanner->takeNextToken();
}


ImportStatement*
Parser::parseImportStatement()
{
    auto importStatement = createSyntax<ImportStatement>(SyntaxKind::ImportStatement);
    expectToken(Token::StringLiteral);
    auto stringLiteral = createSyntax<StringLiteral>(SyntaxKind::StringLiteral);
    finishSyntax(stringLiteral);
    Utf8StringView source(stringLiteral->start + 1, stringLiteral->end - 1);
    Path* newCurrenctDirectory = new Path(_currentDirectory->toString().toString());
    newCurrenctDirectory->add(source.toString());
    _compiler->addFile(newCurrenctDirectory->toString().asString());
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
    expectToken(Token::Name);
    exportStatement->usage = parseModuleAccessUsageOnIdentifier();
    return exportStatement;
}


ModuleDeclaration*
Parser::parseModuleDeclaration()
{
    ModuleDeclaration* module = createDeclaration<ModuleDeclaration>(SyntaxKind::ModuleDeclaration);
    module->name = parseName();
    expectToken(Token::Colon);
    while(true)
    {
        Syntax* statement = parseModuleLevelStatement();
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
Parser::hasEqualIdentifier(Name *id1, Name* id2)
{
    return id1->name == id2->name;
}

EndStatement*
Parser::parseEndStatement()
{
    EndStatement* endStatement = createSyntax<EndStatement>(SyntaxKind::EndStatement);
    endStatement->name = parseName();
    finishSyntax(endStatement);
    expectToken(Token::SemiColon);
    return endStatement;
}

ModuleAccessUsage*
Parser::parseModuleAccessUsageOnIdentifier()
{
    ModuleAccessUsage* moduleAccessUsage = createSyntax<ModuleAccessUsage>(SyntaxKind::ModuleAccessUsage);
    moduleAccessUsage->name = createName();
    expectToken(Token::DoubleColon);
    Token peek = peekNextToken();
    if (peek == Token::Name)
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
                throw ExpectedTokenError(Token::Name, getTokenValue());
            }
            token = takeNextToken();
        }
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
        }
        namedUsage->names.add(createName());
        isInitialUsage = false;
    }
    finishSyntax(namedUsage);
    expectToken(Token::SemiColon);
    return namedUsage;
}


ObjectDeclaration*
Parser::parseObjectDeclaration()
{
    ObjectDeclaration* objectDeclaration = createDeclaration<ObjectDeclaration>(SyntaxKind::ObjectDeclaration);
    _scanner->setFlag(TREAT_STRING_KEYWORD_AS_NAME);
    Token token = takeNextToken();
    if (token == Token::Name || token == Token::StringKeyword)
    {
        objectDeclaration->name = createName();
    }
    else
    {
        throw ExpectedTokenError(Token::Name, getTokenValue());
    }
    expectToken(Token::OpenBrace);
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
        }
        Name* name = createName();
        if (objectDeclaration->kind == SyntaxKind::ObjectDeclaration && name->name == objectDeclaration->name->name)
        {
            objectDeclaration->constructor = parseConstructorDeclaration();
            expectToken(Token::SemiColon);
        }
        else
        {
            objectDeclaration->properties.add(parsePropertyOnIdentifier(name));
        }
    }
    finishSyntax(objectDeclaration);
    _scanner->resetFlags();
    addSymbol(objectDeclaration);
    return objectDeclaration;
}


InterfaceDeclaration*
Parser::parseInterfaceDeclaration()
{
    auto interface = createDeclaration<InterfaceDeclaration>(SyntaxKind::InterfaceDeclaration);
    interface->name = parseName();
    expectToken(Token::OpenBrace);
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
        }
        interface->properties.add(parsePropertyOnIdentifier(nullptr));
    }
    finishSyntax(interface);
    return interface;
}


ContextDeclaration*
Parser::parseContextDeclaration()
{
    auto context = createDeclaration<ContextDeclaration>(SyntaxKind::InterfaceDeclaration);
    context->name = parseName();
    expectToken(Token::OpenBrace);
    while (true)
    {
        Token token = takeNextToken();
        if (token == Token::CloseBrace)
        {
            break;
        }
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
        }
        context->properties.add(parsePropertyOnIdentifier(nullptr));
    }
    addSymbol(context);
    finishSyntax(context);
    return context;
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


ConstructorImplementation*
Parser::parseConstructorImplementation()
{
    auto impl = createDeclaration<ConstructorImplementation>(SyntaxKind::ConstructorImplementation);
    _scanner->setFlag(TREAT_STRING_KEYWORD_AS_NAME);
    impl->name = parseName();
    auto result = parseParameterList();;
    impl->parameterList = result.parameterList;
    Token token = takeNextToken();
    if (token == Token::Colon)
    {
        impl->initializationList = parseInitializationList();
    }
    impl->body = parseFunctionBodyOnOpenBrace();
    finishSyntax(impl);
    addSymbol(impl);
    _scanner->resetFlags();
    return impl;
}


List<CallExpression*>*
Parser::parseInitializationList()
{
    expectToken(Token::Name);
    auto list = new List<CallExpression*>();
    while (true)
    {
        list->add(parseCallExpressionOnName());
        Token token = takeNextToken();
        if (token == Token::Comma)
        {
            Token peek = peekNextToken();
            if (peek == Token::Name)
            {
                skipNextToken();
                continue;
            }
            if (peek == Token::OpenBrace)
            {
                break;
            }
            throw ExpectedTokenError(Token::OpenBrace, getTokenValue());
        }
        else if (token == Token::OpenBrace)
        {
            break;
        }
        throw ExpectedTokenError(Token::OpenBrace, getTokenValue());
    }
    return list;
}


PropertyDeclaration*
Parser::parsePropertyOnIdentifier(Name* name)
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
    expectToken(Token::SemiColon);
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
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
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
    auto domain = createDeclaration<DomainDeclaration>(SyntaxKind::DomainDeclaration);
    expectToken(Token::DoubleColon);
    Token token = takeNextToken();
    if (token != Token::StartKeyword && token != Token::EndKeyword)
    {
        throw ExpectedTokenError(Token::StartKeyword, getTokenValue());
    }
    token = takeNextToken();
    char* start = _scanner->getStartPosition();
    while (true)
    {
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
        }
        domain->names.add(createName());
        char* end = _scanner->getPosition();
        token = takeNextToken();
        if (token == Token::SemiColon)
        {
            auto name = createSyntax<Name>(SyntaxKind::Name);
            name->start = start;
            name->end = end;
            name->name = Utf8StringView(start, end);
            domain->name = name;
            addSymbol(domain);
            finishSyntax(domain);
            return domain;
        }
        if (token != Token::DoubleColon)
        {
            token = takeNextToken();
            throw ExpectedTokenError(Token::DoubleColon, getTokenValue());
        }
    }
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
        if (token != Token::Name)
        {
            throw ExpectedTokenError(Token::Name, getTokenValue());
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
        expectToken(Token::Name);
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
    auto symbol = new Symbol(SymbolSectionIndex::Text, _symbolOffset, declaration->name->name);
    symbols->add(symbol);
    declaration->symbol = symbol;
}


AddressOfExpression*
Parser::parseAddressOfExpression()
{
    auto addressOfExpression = createSyntax<AddressOfExpression>(SyntaxKind::AddressOfExpression);
    addressOfExpression->expression = parseExpression();
    finishSyntax(addressOfExpression);
    return addressOfExpression;
}


}