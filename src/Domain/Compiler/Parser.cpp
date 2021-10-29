#include "Parser.h"
#include "Exceptions.h"
#include <filesystem>
#include <cassert>


namespace fs = std::filesystem;


namespace elet::domain::compiler::ast
{
    thread_local
    const char*
    Parser::_lastStatementLocationEnd = nullptr;


    thread_local
    List<Symbol*>*
    Parser::symbols = nullptr;


    thread_local
    std::uint64_t
    Parser::_symbolOffset = 0;


    Parser::Parser(std::map<std::string, ast::SourceFile*>& files):
        _files(files),
        _scanner(nullptr),
        _error(new ParserError(this))
    {

    }


    ParseResult
    Parser::performWork(SourceFile* sourceFile)
    {
        _sourceFile = sourceFile;
//        Utf8StringView sourceContent(task.sourceStart, task.sourceEnd);
        Scanner scanner(sourceFile);
//        _currentDirectory = const_cast<fs::path*>(task.sourceDirectory);
        _scanner = &scanner;
        List<Syntax*> statements;
//        ParsingTask* pendingParsingTask = nullptr;
        while (true)
        {
            try
            {
                Syntax* statement = parseFileLevelDeclarations();
                if (!statement)
                {
                    break;
                }
                addSymbolToSourceFile(statement, sourceFile);
                statements.add(statement);
            }
            catch (error::UnexpectedEndOfFile &ex)
            {

            }
        }
        return { statements, _syntaxErrors, _lexicalErrors };
    }


    void
    Parser::addSymbolToSourceFile(Syntax* statement, SourceFile* sourceFile)
    {
        if (statement->labels & LABEL__DECLARATION)
        {
            Declaration* declaration = reinterpret_cast<Declaration*>(statement);
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
            }
            declaration->sourceFile = sourceFile;
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
        }
        return block;
    }

    Syntax*
    Parser::parseDomainLevelStatements()
    {
        Token token = _scanner->takeNextToken(false);
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
                throw _error->throwSyntaxError("Unknown domain-level statement.");
        }
    }


    Syntax*
    Parser::parseFileLevelDeclarations()
    {
        Token token = _scanner->takeNextToken(false);
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
                _error->throwSyntaxError("Unknown file-level statement.");
        }
    }


    Syntax*
    Parser::parseFunctionLevelStatement()
    {
        Token token = _scanner->takeNextToken(false);

        try
        {
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
                    Token peek = peekNextToken(false);
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
                    _error->throwSyntaxError("Single variable expressions are disallowed.");
                }
                case Token::EndOfFile:
                    return createSyntax<EndOfFile>(SyntaxKind::EndOfFile);
                default:
                    _error->throwSyntaxError("");
            }
        }
        catch (error::SyntaxError* syntaxError)
        {
            _syntaxErrors.add(syntaxError);
        }
        catch (error::LexicalError* lexicalError)
        {
            _lexicalErrors.add(lexicalError);
        }
        return nullptr;
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
            throw _error->createExpectedTokenError(Token::DoubleQuote);
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
            token = takeNextToken();
        }
        if (token == Token::SemiColon)
        {
            functionDeclaration->body = nullptr;
            goto finishDeclaration;
        }
        else if (token != Token::OpenBrace)
        {
            throw _error->createExpectedTokenError(Token::OpenBrace);
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
        Token peek = peekNextToken(false);
        if (peek == Token::Colon)
        {
            skipNextToken();
            variableDeclaration->type = parseType();
            peek = peekNextToken(false);
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
                throw _error->createExpectedTokenError(Token::Identifier);
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
        while (peekNextToken(false) != Token::CloseBrace)
        {
            Syntax* statement = parseFunctionLevelStatement();

            // Null means it encounters errors but should continue.
            if (!statement)
            {
                continue;
            }
            if (statement->kind == SyntaxKind::EndOfFile)
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


    Utf8StringView
    Parser::getTokenValue()
    {
        return _scanner->getTokenValue();
    }


    void
    Parser::expectToken(Token expected)
    {
        Token result = _scanner->takeNextToken(false);
        assertToken(result, expected);
    }


    void
    Parser::assertToken(Token target, Token expected)
    {
        if (target != expected)
        {
            throw new error::SyntaxError(_error->createErrorNodeOnCurrentToken(), _sourceFile, "Expected '{0}', instead got '{1}'.", eletTokenToString.get(expected), getTokenValue().toString());
        }
    }


    Token
    Parser::peekNextToken(bool includeWhitespaceToken)
    {
        return _scanner->peekNextToken(false);
    }


    Token
    Parser::takeNextToken()
    {
        return _scanner->takeNextToken(false);
    }


    Token
    Parser::takeNextToken(bool includeWhitespaceToken)
    {
        return _scanner->takeNextToken(includeWhitespaceToken);
    }


    TypeAssignment*
    Parser::parseType()
    {
        TypeAssignment* typeAssignment = createSyntax<TypeAssignment>(SyntaxKind::Type);
        Token token = _scanner->takeNextToken(false);
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
            case Token::StringKeyword:
            case Token::Identifier:
                typeAssignment->type = TypeKind::Custom;
                break;
            default:
                throw _error->throwSyntaxError("Expected type annotation.");
        }
        typeAssignment->name = createName();
        while (true)
        {
            Token peek = peekNextToken(false);
            if (peek == Token::Asterisk)
            {
                if (typeAssignment->isLiteral)
                {
                    _error->createExpectedTokenError(Token::CloseParen);
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
        Token peek = peekNextToken(false);

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
                peek = peekNextToken(false);
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
        Token peek = peekNextToken(false);
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
                StringLiteral *stringLiteral = createSyntax<StringLiteral>(SyntaxKind::StringLiteral);
                finishSyntax(stringLiteral);
                stringLiteral->stringStart = stringLiteral->start + 1;
                stringLiteral->stringEnd = stringLiteral->end - 1;
                return stringLiteral;
            }
            case Token::Minus:
                return createNegativeIntegerLiteral();
            case Token::HexadecimalLiteral:
            case Token::DecimalLiteral:
                return createIntegerLiteral(token);
            case Token::TrueKeyword:
                return createBooleanLiteral(true);
            case Token::FalseKeyword:
                return createBooleanLiteral(false);
            case Token::OpenBracket:
                return parseArrayLiteral();
            case Token::OpenParen:
                return parseTuple();
            case Token::Identifier:
                return parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();
        }
        throw _error->throwSyntaxError("Expected expression.");
    }


    IntegerLiteral*
    Parser::createNegativeIntegerLiteral()
    {
        Token token = takeNextToken();
        if (token == Token::HexadecimalLiteral || token == Token::DecimalLiteral)
        {
            IntegerLiteral* integerLiteral = createIntegerLiteral(token);
            integerLiteral->isNegative = true;
            return integerLiteral;
        }
        else
        {
            throw std::runtime_error("Not implemented syntax.");
        }
    }


    IntegerLiteral*
    Parser::createIntegerLiteral(Token& token)
    {
        IntegerLiteral* integerLiteral = createSyntax<IntegerLiteral>(SyntaxKind::IntegerLiteral);
        switch (token)
        {
            case Token::DecimalLiteral:
            {
                DecimalLiteral* decimalLiteral = createSyntax<DecimalLiteral>(SyntaxKind::DecimalLiteral);
                finishSyntax(decimalLiteral);
                integerLiteral->digits = decimalLiteral;
                break;
            }
            case Token::HexadecimalLiteral:
            {
                HexadecimalLiteral* hexadecimalLiteral = createSyntax<HexadecimalLiteral>(SyntaxKind::HexadecimalLiteral);
                finishSyntax(hexadecimalLiteral);
                integerLiteral->digits = hexadecimalLiteral;
                break;
            }
        }
        Token suffixPeek = peekNextToken(true);
        if (isIntegerSuffix(suffixPeek))
        {
            takeNextToken();
            integerLiteral->suffix = createSyntax<IntegerSuffix>(SyntaxKind::IntegerSuffix);
            integerLiteral->suffix->type = suffixPeek;
        }
        else
        {
            integerLiteral->suffix = nullptr;
        }
        switch (token)
        {
            case Token::DecimalLiteral:
                integerLiteral->value = parseDecimalLiteral(std::get<DecimalLiteral*>(integerLiteral->digits), getIntegerMaxLimitFromToken(suffixPeek, IntegerLimit::S32Max));
                break;
            case Token::HexadecimalLiteral:
                integerLiteral->value = parseHexadecimalLiteral(std::get<HexadecimalLiteral*>(integerLiteral->digits), getIntegerMaxLimitFromToken(suffixPeek, IntegerLimit::U64Max));
                break;
        }
        finishSyntax(integerLiteral);
        return integerLiteral;
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
        Token peek = peekNextToken(false);
        while (true)
        {
            if (peek == Token::CloseParen)
            {
                skipNextToken();
                break;
            }

            Expression* expression = parseExpression();
            tuple->values.add(expression);
            peek = peekNextToken(false);
            if (peek == Token::Comma)
            {
                skipNextToken();
                peek = peekNextToken(false);
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
                throw _error->createExpectedTokenError(Token::OpenBrace);
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
        Token peek = peekNextToken(false);
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
        _scanner->takeNextToken(false);
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
                    throw _error->createExpectedTokenError(Token::Identifier);
                }
                token = takeNextToken();
            }
            if (token == Token::CloseBrace)
            {
                break;
            }
            if (token != Token::Identifier)
            {
                throw _error->createExpectedTokenError(Token::Identifier);
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
            throw _error->createExpectedTokenError(Token::Identifier);
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
                throw _error->createExpectedTokenError(Token::Identifier);
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
                throw _error->createExpectedTokenError(Token::Identifier);
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
        Token peek = peekNextToken(false);
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
            if (token == Token::CloseBrace)
            {
                break;
            }
            if (token != Token::Identifier)
            {
                _error->createExpectedTokenError(Token::Identifier);
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
                _error->createExpectedTokenError(Token::Identifier);
            }
            _currentDomain->names.add(createName());
            char *end = _scanner->getPositionAddress();
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
                _error->createExpectedTokenError(Token::DoubleColon);
            }
            token = takeNextToken();
        }

    setAccessMap:
        std::size_t last = _currentDomain->names.size() - 1;
        DomainDeclarationMap* currentAccessMap = &domainDeclarationMap;
        for (int i = 0; i <= last; i++)
        {
            auto name = _currentDomain->names[i];
            if (i == last)
            {
                currentAccessMap->insert({ name->name, &_currentDomain->block->symbols });
                break;
            }
            DomainDeclarationMap* newAccessMap = new DomainDeclarationMap();
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
                _error->createExpectedTokenError(Token::Identifier);
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
        Token peek = peekNextToken(false);
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
        Token peek = peekNextToken(false);
        while (true)
        {
            if (peek == Token::CloseBracket)
            {
                skipNextToken();
                break;
            }
            Expression* expression = parseExpression();
            arrayLiteral->values.add(expression);
            peek = peekNextToken(false);
            if (peek == Token::Comma)
            {
                skipNextToken();
                peek = peekNextToken(false);
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
            throw _error->throwSyntaxError("Onyl C language API is allowed.");
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
                throw error::UnexpectedEndOfFile();
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


    uint64_t
    Parser::parseHexadecimalLiteral(const HexadecimalLiteral* hexadecimalLiteral, IntegerLimit maxLimit) const
    {
        const char* cursor = hexadecimalLiteral->end - 1;
        const char* start = hexadecimalLiteral->start + 2;
        uint64_t exponent = 0;
        uint64_t result = 0;
        uint64_t leftOfMaxLimit = static_cast<uint64_t>(maxLimit);
        while (true)
        {
            unsigned char character = cursor[0];
            uint64_t positionValue;
            if (character >= '0' && character <= '9')
            {
                positionValue = character - '0';
            }
            else if (character >= 'a' && character <= 'f')
            {
                positionValue = (character - 'a') + 10;
            }
            else if (character == '_')
            {
                cursor--;
                continue;
            }
            else
            {
                assert("Unknown character in hexadecimal literal.");
            }
            uint64_t f = positionValue * std::pow(16ui64, exponent);
            if (f > leftOfMaxLimit)
            {
                _error->throwSyntaxError(
                    hexadecimalLiteral,
                    "Integer literal has exceeded its max limit of {0}({1}).",
                    toStringFromIntegerLimit(maxLimit), static_cast<uint64_t>(maxLimit));
            }
            result += f;
            leftOfMaxLimit -= f;
            if (cursor == start)
            {
                break;
            }
            cursor--;
            exponent++;
        }
        return result;
    }


    uint64_t
    Parser::parseDecimalLiteral(const DecimalLiteral* decimalLiteral, IntegerLimit maxLimit) const
    {
        const char* cursor = decimalLiteral->end - 1;
        unsigned int exponent = 0;
        uint64_t result = 0;
        uint64_t leftOfMaxLimit = static_cast<uint64_t>(maxLimit);
        while (true)
        {
            unsigned int s = cursor[0] - '0';
            if (s == '_')
            {
                cursor--;
                continue;
            }
            unsigned int f = s * std::pow(10, exponent);
            if (f > leftOfMaxLimit)
            {
                _error->throwSyntaxError(
                    decimalLiteral,
                    "Integer literal has exceeded its max limit of {0}({1}).",
                    toStringFromIntegerLimit(maxLimit), static_cast<uint64_t>(maxLimit));
            }
            result += f;
            leftOfMaxLimit -= f;
            if (cursor == decimalLiteral->start)
            {
                break;
            }
            cursor--;
            exponent++;
        }
        return result;
    }


    unsigned int
    Parser::getDigitsLength(const char* end, const char* start) const
    {
        int length = 0;
        while (true)
        {
            if (end[0] == '_')
            {
                end--;
                continue;
            }
            if (end == start)
            {
                break;
            }
            end--;
            length++;
        }
        return length;
    }





    IntegerLimit
    Parser::getIntegerMaxLimitFromToken(Token token, IntegerLimit defaultLimit) const
    {
        switch (token)
        {
            case Token::U8Keyword:
                return IntegerLimit::U8Max;
            case Token::U16Keyword:
                return IntegerLimit::U16Max;
            case Token::U32Keyword:
                return IntegerLimit::U32Max;
            case Token::U64Keyword:
                return IntegerLimit::U64Max;
            case Token::S8Keyword:
                return IntegerLimit::S8Max;
            case Token::S16Keyword:
                return IntegerLimit::S16Max;
            case Token::S32Keyword:
                return IntegerLimit::S32Max;
            case Token::S64Keyword:
                return IntegerLimit::S64Max;
            default:
                return defaultLimit;
        }
    }


    std::string
    Parser::toStringFromIntegerLimit(IntegerLimit limit) const
    {
        switch (limit)
        {
            case IntegerLimit::U8Max:
                return "u8";
            case IntegerLimit::U16Max:
                return "u16";
            case IntegerLimit::U32Max:
                return "u32";
            case IntegerLimit::U64Max:
                return "u64";

            case IntegerLimit::S8Max:
                return "s8";
            case IntegerLimit::S16Max:
                return "s16";
            case IntegerLimit::S32Max:
                return "s32";
            case IntegerLimit::S64Max:
                return "s64";
        }
    }


    bool
    Parser::isIntegerSuffix(Token token)
    {
        return token >= Token::U8Keyword && token <= Token::S64Keyword;
    }


    void
    Parser::skipToNextSemicolon()
    {
        Token token;
        do
        {
            token = takeNextToken();
        }
        while(token != Token::SemiColon && token != Token::EndOfFile);
    }
}