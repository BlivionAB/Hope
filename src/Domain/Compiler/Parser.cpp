#include "Parser.h"
#include "Domain/Compiler/Error/Error_Syntax.h"
#include <Foundation/Utf8String.h>
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
        Scanner scanner(sourceFile);
        _scanner = &scanner;
        List<Syntax*> statements;
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
            catch (error::SyntaxError &ex)
            {
                std::cout << ex.message << std::endl;
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
                sourceFile->declarations.insert({ declaration->symbol->name, declaration });
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
                throw _error->throwSyntaxError<error::UnknownDomainLevelStatement>();
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
                _error->throwSyntaxError<error::UnknownFileLevelStatement>();
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
                    _error->throwSyntaxError<error::UnknownFunctionLevelStatement>();
                }
                case Token::EndOfFile:
                    return createSyntax<EndOfFile>(SyntaxKind::EndOfFile);
                default:
                    _error->throwSyntaxError<error::UnknownFunctionLevelStatement>();
            }
        }
        catch (error::SyntaxError* syntaxError)
        {
            _syntaxErrors.add(syntaxError);
            _scanner->scanToNextSemicolon();
        }
        catch (error::LexicalError* lexicalError)
        {
            _lexicalErrors.add(lexicalError);
            _scanner->scanToNextSemicolon();
        }
        return nullptr;
    }


    FunctionDeclaration*
    Parser::parseFunctionDeclaration()
    {
        auto functionDeclaration = createDeclaration<FunctionDeclaration>(SyntaxKind::FunctionDeclaration);
        functionDeclaration->isCReference = _isInCBlock;
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
        expectToken(Token::Colon);
        variableDeclaration->type = parseType();
        Token peek = peekNextToken(false);
        if (peek == Token::Equal)
        {
            skipNextToken();
            variableDeclaration->expression = parseExpression(false);
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
            ErrorNode* errorNode = _error->createErrorNodeOnCurrentToken();
            const char* tokenValue = getTokenValue().toString();
            throw new error::UnexpectedTokenError(_sourceFile, errorNode, eletTokenToString.get(expected), tokenValue);
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
            case Token::CharKeyword:
                typeAssignment->type = TypeKind::Char;
                break;
            case Token::VoidKeyword:
                typeAssignment->type = TypeKind::Void;
                break;
            case Token::BoolKeyword:
                typeAssignment->type = TypeKind::Bool;
                break;
            case Token::U8Keyword:
                typeAssignment->type = TypeKind::U8;
                break;
            case Token::S8Keyword:
                typeAssignment->type = TypeKind::S8;
                break;
            case Token::U16Keyword:
                typeAssignment->type = TypeKind::U16;
                break;
            case Token::S16Keyword:
                typeAssignment->type = TypeKind::S16;
                break;
            case Token::U32Keyword:
                typeAssignment->type = TypeKind::U32;
                break;
            case Token::S32Keyword:
                typeAssignment->type = TypeKind::S32;
                break;
            case Token::U64Keyword:
                typeAssignment->type = TypeKind::U64;
                break;
            case Token::S64Keyword:
                typeAssignment->type = TypeKind::S64;
                break;
            case Token::StringKeyword:
            case Token::Identifier:
                typeAssignment->type = TypeKind::Custom;
                break;
            default:
                _error->throwSyntaxError<error::ExpectedTypeAnnotationError>(getTokenValue().toString());
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
    Parser::parseExpression(bool skipNextOperator)
    {
        Token token = takeNextToken();
        Expression* expr = parseExpressionOnToken(token);
        Token peek = peekNextToken(false);

        TypeCast* previousTypeCast = nullptr;
        while (peek == Token::AsKeyword)
        {
            TypeCast* typeCast = createSyntax<TypeCast>(SyntaxKind::TypeCast);
            takeNextToken();
            typeCast->type = parseType();
            finishSyntax(typeCast);
            if (!expr->typeCast)
            {
                expr->typeCast = typeCast;
            }
            peek = peekNextToken(false);
            if (previousTypeCast)
            {
                previousTypeCast->typeCast = typeCast;
            }
            previousTypeCast = typeCast;
        }
        if (isBinaryOperator(peek) && !skipNextOperator)
        {
            return parseBinaryExpression(expr, peek);
        }
        return expr;
    }


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
    Expression*
    Parser::parseBinaryExpression(Expression* expression, Token& peek)
    {
        Expression* leftExpression = expression;
        int previousPrecedence = 0;
        while (isBinaryOperator(peek))
        {
            BinaryExpression* binaryExpression = createSyntax<BinaryExpression>(SyntaxKind::BinaryExpression);
            binaryExpression->start = leftExpression->start;
            unsigned int precedence = getOperatorPrecedence(peek);
            takeNextToken();
            binaryExpression->left = leftExpression;
            binaryExpression->binaryOperator = createSyntax<BinaryOperator>(SyntaxKind::BinaryOperator);
            finishSyntax(binaryExpression->binaryOperator);
            binaryExpression->binaryOperatorKind = getBinaryOperatorKind(peek);
            binaryExpression->right = parseRightHandSideOfBinaryExpression(precedence);
            finishSyntax(binaryExpression);
            leftExpression = binaryExpression;
            peek = peekNextToken(false);
        }
        return leftExpression;
    }


    Expression*
    Parser::parseRightHandSideOfBinaryExpression(unsigned int previousOperatorPrecedence)
    {
        Expression* expression = parseExpression(true);
        Token peek = peekNextToken(false);
        if (isBinaryOperator(peek))
        {
            unsigned int nextOperatorPrecedence = getOperatorPrecedence(peek);
            if (nextOperatorPrecedence > previousOperatorPrecedence)
            {
                takeNextToken();

                BinaryExpression* binaryExpression = createSyntax<BinaryExpression>(SyntaxKind::BinaryExpression);
                binaryExpression->start = expression->start;
                binaryExpression->left = expression;
                binaryExpression->binaryOperatorKind = getBinaryOperatorKind(peek);
                binaryExpression->binaryOperator = createSyntax<BinaryOperator>(SyntaxKind::BinaryOperator);
                finishSyntax(binaryExpression->binaryOperator);
                binaryExpression->operatorPrecedence = nextOperatorPrecedence;
                binaryExpression->right = parseRightHandSideOfBinaryExpression(nextOperatorPrecedence);
                finishSyntax(binaryExpression);
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
//            case Token::CharacterLiteral:
//                return createCharacterLiteral();
            case Token::Minus:
                return createNegativeIntegerLiteral();
            case Token::HexadecimalLiteral:
            case Token::DecimalLiteral:
            case Token::BinaryLiteral:
                return createIntegerLiteral(token);
            case Token::TrueKeyword:
                return createBooleanLiteral(true);
            case Token::FalseKeyword:
                return createBooleanLiteral(false);
            case Token::OpenBracket:
                return parseArrayLiteral();
            case Token::OpenParen:
                return parseParenExpression();
            case Token::Identifier:
                return parseModuleAccessOrPropertyAccessOrCallExpressionOnIdentifier();
        }
        throw _error->throwSyntaxError<error::ExpectedExpressionError>();
    }


    IntegerLiteral*
    Parser::createNegativeIntegerLiteral()
    {
        Token token = takeNextToken();
        if (token == Token::HexadecimalLiteral || token == Token::DecimalLiteral)
        {
            IntegerLiteral* integerLiteral = createIntegerLiteral(token, true);
            integerLiteral->isNegative = true;
            integerLiteral->value = -integerLiteral->value;
            finishSyntax(integerLiteral);
            return integerLiteral;
        }
        else
        {
            throw std::runtime_error("Not implemented syntax.");
        }
    }


    CharacterLiteral*
    Parser::createCharacterLiteral()
    {
        using Character = Utf8String::Character;

        CharacterLiteral* characterLiteral = createSyntax<CharacterLiteral>(SyntaxKind::CharacterLiteral);
        characterLiteral->value = _scanner->getCharacterLiteralValue();
        finishSyntax(characterLiteral);
        return characterLiteral;
    }


    IntegerLiteral*
    Parser::createIntegerLiteral(Token& token)
    {
        return createIntegerLiteral(token, false);
    }


    IntegerLiteral*
    Parser::createIntegerLiteral(Token& token, bool isNegative)
    {
        IntegerLiteral* integerLiteral = createSyntax<IntegerLiteral>(SyntaxKind::IntegerLiteral);
        switch (token)
        {
            case Token::BinaryLiteral:
            {
                BinaryLiteral* binaryLiteral = createSyntax<BinaryLiteral>(SyntaxKind::BinaryLiteral);
                finishSyntax(binaryLiteral);
                integerLiteral->digits = binaryLiteral;
                break;
            }
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
            case Token::BinaryLiteral:
                integerLiteral->value = parseBinaryLiteral(std::get<BinaryLiteral*>(integerLiteral->digits), isNegative);
                break;
            case Token::DecimalLiteral:
                integerLiteral->value = parseDecimalLiteral(std::get<DecimalLiteral*>(integerLiteral->digits), isNegative);
                break;
            case Token::HexadecimalLiteral:
                integerLiteral->value = parseHexadecimalLiteral(std::get<HexadecimalLiteral*>(integerLiteral->digits), isNegative);
                break;
        }
        finishSyntax(integerLiteral);
        return integerLiteral;
    }


    BooleanLiteral*
    Parser::createBooleanLiteral(bool value)
    {
        BooleanLiteral* boolean = createSyntax<BooleanLiteral>(SyntaxKind::BooleanLiteral);
        boolean->value = value;
        finishSyntax(boolean);
        return boolean;
    }


    ParenExpression*
    Parser::parseParenExpression()
    {
        ParenExpression* parenExpression = createSyntax<ParenExpression>(SyntaxKind::ParenExpression);
        parenExpression->expression = parseExpression(false);
        expectToken(Token::CloseParen);
        finishSyntax(parenExpression);
        return parenExpression;
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
        _currentDomain->names = createSyntax<SyntaxList<Name*>>(SyntaxKind::SyntaxList);

        while (true)
        {
            if (token != Token::Identifier)
            {
                _error->createExpectedTokenError(Token::Identifier);
            }
            _currentDomain->names->add(createName());
            finishSyntax(static_cast<Syntax*>(_currentDomain->names));
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
        std::size_t last = _currentDomain->names->size() - 1;
        DomainDeclarationMap* currentAccessMap = &domainDeclarationMap;
        for (int i = 0; i <= last; i++)
        {
            auto name = (*_currentDomain->names)[i];
            if (i == last)
            {
                currentAccessMap->insert({ name->name, &_currentDomain->block->symbols });
                break;
            }
            DomainDeclarationMap* newAccessMap = new DomainDeclarationMap();
            currentAccessMap->insert({ name->name, newAccessMap });
            currentAccessMap = newAccessMap;
        }
        finishSyntax(_currentDomain);
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
            property->value = parseExpression(false);
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
            Token peek = peekNextToken(false);
            finishSyntax(moduleAccessExpression);
            return moduleAccessExpression;
        }
        else if (peek == Token::AsKeyword)
        {
            // Pass in Token::Unknown, so we can bypass dot and call expressions.
            return createPropertyAccessExpressionOrCallExpressionFromPeek(Token::Unknown);
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
            Expression* expression = parseExpression(false);
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
    Parser::seek(const TextScanner::Location& location)
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
        addressOfExpression->expression = parseExpression(false);
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
            throw _error->throwSyntaxError<error::ExpectedCDeclarationError>();
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
        ifStatement->condition = parseExpression(false);
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
            case Token::Ampersand:
                return BinaryOperatorKind::BitwiseAnd;
            case Token::Caret:
                return BinaryOperatorKind::BitwiseXor;
            case Token::Pipe:
                return BinaryOperatorKind::BitwiseOr;
            case Token::EqualEqual:
                return BinaryOperatorKind::Equal;
            case Token::Plus:
                return BinaryOperatorKind::Add;
            case Token::Minus:
                return BinaryOperatorKind::Minus;
            case Token::Asterisk:
                return BinaryOperatorKind::Multiply;
            case Token::ForwardSlash:
                return BinaryOperatorKind::Divide;
            case Token::Percent:
                return BinaryOperatorKind::Modulo;

            case Token::AsKeyword:
                return BinaryOperatorKind::TypeCast;

            default:
                throw std::runtime_error("Unknown binary operator.");
        }
    }

    ReturnStatement*
    Parser::parseReturnStatement()
    {
        ReturnStatement* returnStatement = createSyntax<ReturnStatement>(SyntaxKind::ReturnStatement);
        returnStatement->expression = parseExpression(false);
        expectToken(Token::SemiColon);
        return returnStatement;
    }


    uint8_t
    Parser::getOperatorPrecedence(Token token) const
    {
        switch (token)
        {
            case Token::Asterisk:
            case Token::ForwardSlash:
            case Token::Percent:
                return 7;
            case Token::Minus:
            case Token::Plus:
                return 6;
            case Token::Ampersand:
                return 5;
            case Token::Caret:
                return 4;
            case Token::Pipe:
                return 3;
            case Token::AmpersandAmpersand:
                return 2;
            case Token::PipePipe:
                return 1;
            default:
                throw std::runtime_error("Unknown token in getOperatorPrecedence.");
        }
    }


    bool
    Parser::isBinaryOperator(Token token) const
    {
        return token >= Token::BinaryOperationStart && token <= Token::BinaryOperationEnd;
    }


    uint64_t
    Parser::parseBinaryLiteral(const BinaryLiteral* binaryLiteral, bool isNegative) const
    {
        const char* cursor = binaryLiteral->end - 1;
        const char* start = binaryLiteral->start + 2;
        uint64_t exponent = 0;
        uint64_t result = 0;
        uint64_t leftOfMaxLimit = isNegative ? static_cast<uint64_t>(INT64_MAX) + 1 : UINT64_MAX;
        while (true)
        {
            unsigned char character = cursor[0];
            if (character == '_')
            {
                cursor--;
                continue;
            }
            uint64_t positionValue = character - '0';
            uint64_t f = positionValue << exponent;
            if (f > leftOfMaxLimit)
            {
                if (isNegative)
                {
                    _error->throwSyntaxErrorWithNode<error::IntegerLiteralUnderflowError>(binaryLiteral);
                }
                else
                {
                    _error->throwSyntaxErrorWithNode<error::IntegerLiteralOverflowError>(binaryLiteral);
                }
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
    Parser::parseHexadecimalLiteral(const HexadecimalLiteral* hexadecimalLiteral, bool isNegative) const
    {
        const char* cursor = hexadecimalLiteral->end - 1;
        const char* start = hexadecimalLiteral->start + 2;
        uint64_t exponent = 0;
        uint64_t result = 0;
        uint64_t leftOfMaxLimit = isNegative ? static_cast<uint64_t>(INT64_MAX) + 1 : UINT64_MAX;
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
                if (isNegative)
                {
                    _error->throwSyntaxErrorWithNode<error::IntegerLiteralUnderflowError>(hexadecimalLiteral);
                }
                else
                {
                    _error->throwSyntaxErrorWithNode<error::IntegerLiteralOverflowError>(hexadecimalLiteral);
                }
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
    Parser::parseDecimalLiteral(const DecimalLiteral* decimalLiteral, bool isNegative) const
    {
        const char* cursor = decimalLiteral->end - 1;
        uint64_t exponent = 0;
        uint64_t result = 0;
        uint64_t leftOfMaxLimit = isNegative ? static_cast<uint64_t>(INT64_MAX) + 1 : UINT64_MAX;
        while (true)
        {
            char character = cursor[0];
            if (character == '_')
            {
                cursor--;
                continue;
            }
            uint64_t s = character - '0';
            uint64_t f = s * std::pow(10, exponent);
            if (f > leftOfMaxLimit)
            {
                if (isNegative)
                {
                    _error->throwSyntaxErrorWithNode<error::IntegerLiteralUnderflowError>(decimalLiteral);
                }
                else
                {
                    _error->throwSyntaxErrorWithNode<error::IntegerLiteralOverflowError>(decimalLiteral);
                }
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