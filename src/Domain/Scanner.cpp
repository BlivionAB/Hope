#include "Scanner.h"

using Token = Scanner::Token;
using namespace elet::foundation;

Scanner::Scanner(const Utf8StringView& source):
    BaseScanner(source)
{

}

Token
Scanner::takeNextToken()
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
                continue;
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
        return *token;
    }
    return Token::Identifier;
}


Utf8StringView
Scanner::getTokenValue() const
{
    return _source.slice(_startMemoryLocationOfToken, _sourceIterator.getPosition());
}


Token
Scanner::peekNextToken()
{
    saveCurrentLocation();
    Token token = takeNextToken();
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
