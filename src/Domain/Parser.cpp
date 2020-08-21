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
    switch (token)
    {
        case Token::VarKeyword:
            return parseVariableDeclaration();
        case Token::AssemblyKeyword:
            return parseAssemblyBlock();
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
                {
                    PropertyAccessExpression *propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
                    propertyAccessExpression->name = createIdentifer();
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
}


FunctionDeclaration*
Parser::parseFunctionDeclaration()
{
    auto functionDeclaration = createDeclaration<FunctionDeclaration>(SyntaxKind::FunctionDeclaration);
    functionDeclaration->name = parseIdentifier();
    auto parameterListResult = parseParameterList();
    functionDeclaration->parameterList = parameterListResult.parameterList;
    expectToken(Token::Colon);
    functionDeclaration->type = parseType();
    functionDeclaration->body = parseFunctionBody();
    addSymbol(functionDeclaration);
    finishSyntax<FunctionDeclaration>(functionDeclaration);
    return functionDeclaration;
}


VariableDeclaration*
Parser::parseVariableDeclaration()
{
    VariableDeclaration* variableDeclaration = createDeclaration<VariableDeclaration>(SyntaxKind::VariableDeclaration);
    variableDeclaration->name = parseIdentifier();
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
    expectToken(Token::SemiColon);
    return variableDeclaration;
}


Name*
Parser::parseIdentifier()
{
    expectToken(Token::Identifier);
    return createIdentifer();
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
    syntax->name = createIdentifer();
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
    TypeAssignment* type = createSyntax<TypeAssignment>(SyntaxKind::Type);
    Token token = _scanner->takeNextToken();
    switch (token) {
        case Token::IntKeyword:
            type->type = TypeKind::Int;
            break;
        case Token::UnsignedIntKeyword:
            type->type = TypeKind::UInt;
            break;
        case Token::CharKeyword:
            type->type = TypeKind::Char;
            break;
        case Token::VoidKeyword:
            type->type = TypeKind::Void;
            break;
        case Token::LengthOfKeyword:
            type->type = TypeKind::Length;
            type->size = 0;
            type->name = createIdentifer();
            type->parameter = parseIdentifier();
            return type;
        case Token::Identifier:
            type->type = TypeKind::Custom;
            break;
        default:
            throw UnknownTypeError();
    }
    type->size = 0; // Size is set in the transformer.
    type->name = createIdentifer();
    while (true)
    {
        Token peek = peekNextToken();
        if (peek == Token::Asterisk)
        {
            skipNextToken();
            type->pointers.add(createPunctuation(PunctuationType::Asterisk));
        }
        else
        {
            break;
        }
    }
    return type;
}


Name*
Parser::createIdentifer()
{
    Name* identifier = createSyntax<Name>(SyntaxKind::Identifier);
    identifier->name = getTokenValue();
    finishSyntax(identifier);
    return identifier;
}


CallExpression*
Parser::parseCallExpressionOnIdentifier()
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
    return parseExpressionOnToken(token);
}


Expression*
Parser::parseExpressionOnToken(Token token)
{
    switch (token)
    {
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
        case Token::Identifier:
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
    return createPropertyAccessExpressionOrCallExpressionFromPeek(peek);
}


Expression*
Parser::createPropertyAccessExpressionOrCallExpressionFromPeek(Token peek)
{
    if (peek == Token::Dot)
    {
        PropertyAccessExpression* propertyAccessExpression = createSyntax<PropertyAccessExpression>(SyntaxKind::PropertyAccessExpression);
        skipNextToken();
        propertyAccessExpression->expression = parsePropertyAccessOrCallExpression();
        finishSyntax(propertyAccessExpression);
        return propertyAccessExpression;
    }
    if (peek == Token::OpenParen)
    {
        return parseCallExpressionOnIdentifier();
    }
    PropertyExpression* propertyExpression = createSyntax<PropertyExpression>(SyntaxKind::PropertyExpression);
    propertyExpression->name = createIdentifer();
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
    ImportStatement* importStatement = createSyntax<ImportStatement>(SyntaxKind::ImportStatement);
    expectToken(Token::StringLiteral);
    StringLiteral* stringLiteral = createSyntax<StringLiteral>(SyntaxKind::StringLiteral);
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
    expectToken(Token::Identifier);
    exportStatement->usage = parseModuleAccessUsageOnIdentifier();
    return exportStatement;
}


ModuleDeclaration*
Parser::parseModuleDeclaration()
{
    ModuleDeclaration* module = createDeclaration<ModuleDeclaration>(SyntaxKind::ModuleDeclaration);
    module->name = parseIdentifier();
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
    InterfaceDeclaration* interface = createDeclaration<InterfaceDeclaration>(SyntaxKind::InterfaceDeclaration);
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
            auto result = parseParameterList();;
            property->parameters = result.parameterList;
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
    EnumDeclaration* enumDeclaration = createDeclaration<EnumDeclaration>(SyntaxKind::EnumDeclaration);
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
        property->name = createIdentifer();
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
        moduleAccessExpression->name = createIdentifer();
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
    ArrayLiteral* arrayLiteral = createSyntax<ArrayLiteral>(SyntaxKind::ArrayLiteral);
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
    expression->reference = parseIdentifier();
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


}