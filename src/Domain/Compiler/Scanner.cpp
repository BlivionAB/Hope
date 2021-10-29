#include "Scanner.h"
#include "Exceptions.h"


namespace elet::domain::compiler::ast
{
    using namespace elet::foundation;


    Scanner::Scanner(const SourceFile* sourceFile):
        BaseScanner(Utf8StringView(sourceFile->start, sourceFile->end)),
        _sourceFile(sourceFile)
    {

    }


    Token
    Scanner::takeNextToken(bool includeWhitespaceToken)
    {
        while (_sourceIterator != _endIterator)
        {
            setTokenStartPosition();
            Character character = getCharacter();
            increment();
            switch (character)
            {
                case Character::CarriageReturn:
                case Character::Newline:
                case Character::Space:
                    if (includeWhitespaceToken)
                    {
                        return scanWhitespace();
                    }
                    continue;
                case Character::_0:
                case Character::_1:
                case Character::_2:
                case Character::_3:
                case Character::_4:
                case Character::_5:
                case Character::_6:
                case Character::_7:
                case Character::_8:
                case Character::_9:
                {
                    Character ch = getCharacter();
                    if (ch == Character::x)
                    {
                        return scanHexadecimalDigits();
                    }
                    return scanDigits();
                }
                case Character::Comma:
                    return Token::Comma;
                case Character::Asterisk:
                    return Token::Asterisk;
                case Character::SingleQuote:
                    return Token::SingleQuote;
                case Character::DoubleQuote:
                    return scanString();
                case Character::Dot:
                    return Token::Dot;
                case Character::Colon:
                    if (getCharacter() == Character::Colon)
                    {
                        increment();
                        return Token::DoubleColon;
                    }
                    return Token::Colon;
                case Character::Semicolon:
                    return Token::SemiColon;
                case Character::OpenBracket:
                    return Token::OpenBracket;
                case Character::CloseBracket:
                    return Token::CloseBracket;
                case Character::OpenParen:
                    return Token::OpenParen;
                case Character::CloseParen:
                    return Token::CloseParen;
                case Character::OpenBrace:
                    return Token::OpenBrace;
                case Character::CloseBrace:
                    return Token::CloseBrace;
                case Character::Ampersand:
                    return Token::Ampersand;
                case Character::Plus:
                    return Token::Plus;
                case Character::Minus:
                    return Token::Minus;
                case Character::Equal:
                    if (getCharacter() == Character::Equal)
                    {
                        increment();
                        return Token::EqualEqual;
                    }
                    return Token::Equal;
                default:
                    if (isIdentifierStart(character))
                    {
                        while (isIdentifierPart(getCharacter()))
                        {
                            increment();
                        }
                        return getTokenFromString(getTokenValue());
                    }
                    return Token::Unknown;
            }
        }
        return Token::EndOfFile;
    }


    Token
    Scanner::getTokenFromString(const Utf8StringView& string) const
    {
        std::optional token = eletStringToToken.find(string);
        if (token)
        {
            if (token == Token::StringKeyword && _stage & TREAT_STRING_KEYWORD_AS_NAME)
            {
                return Token::Identifier;
            }
            return *token;
        }
        return Token::Identifier;
    }


    Utf8StringView
    Scanner::getTokenValue() const
    {
        return _source.slice(_startMemoryLocationOfToken, _sourceIterator.getPositionAddress());
    }


    Token
    Scanner::peekNextToken(bool includeWhitespaceToken)
    {
        saveCurrentLocation();
        Token token = takeNextToken(includeWhitespaceToken);
        revertToSavedLocation();
        return token;
    }


    Token
    Scanner::scanString()
    {
        while (true)
        {
            Character character = getCharacter();
            increment();
            if (character == Character::DoubleQuote || character == Character::EndOfFile)
            {
                break;
            }
        }
        return Token::StringLiteral;
    }

    void
    Scanner::setFlag(std::uint8_t stage)
    {
        _stage = stage;
    }

    void
    Scanner::resetFlags()
    {
        _stage = 0;
    }


    Token
    Scanner::scanDigits()
    {
        while (true)
        {
            Character character = getCharacter();
            if ((character >= Character::_0 && character <= Character::_9) ||
                character == Character::_)
            {
                increment();
                continue;
            }
            break;
        }
        return Token::DecimalLiteral;
    }


    Token
    Scanner::scanHexadecimalDigits()
    {
        increment();
        bool hasAtLeastOneHexCharacter = false;
        while (true)
        {
            Character character = getCharacter();
            if ((character >= Character::_0 && character <= Character::_9)
                || (character >= Character::a && character <= Character::f))
            {
                hasAtLeastOneHexCharacter = true;
                increment();
                continue;
            }
            else if (character == Character::_)
            {
                increment();
                continue;
            }
            break;
        }
        if (!hasAtLeastOneHexCharacter)
        {
            const char* positionAddress = getPositionAddress();
            scanToNextSemicolon();
            throw new error::LexicalError(_sourceFile, positionAddress, "Hexadecimal literal must consist of at least one hexadecimal character.");
        }
        return Token::HexadecimalLiteral;
    }


    Token
    Scanner::scanWhitespace()
    {
        while (true)
        {
            Character character = getCharacter();
            switch (character)
            {
                case Character::Space:
                case Character::Newline:
                case Character::CarriageReturn:
                    increment();
                    continue;
            }
            break;
        }
        return Token::Whitespace;
    }
}
