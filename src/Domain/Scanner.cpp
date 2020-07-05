#include "Scanner.h"
#include "Exceptions.h"
#include <optional>

using Token = Scanner::Token;

Scanner::Scanner(const Utf8StringView& source):
    _source(source),
    _sourceIterator(source.begin()),
    _endIterator(source.end())
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
            case Character::OpenParen:
                return Token::OpenParen;
            case Character::CloseParen:
                return Token::CloseParen;
            case Character::OpenBrace:
                return Token::OpenBrace;
            case Character::CloseBrace:
                return Token::CloseBrace;
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


void
Scanner::setTokenStartPosition()
{
    _startMemeryLocationOfToken = _sourceIterator.getPosition();
    _startColumnOfToken = _column;
    _startLineOfToken = _line;
}

Utf8StringView
Scanner::getTokenValue() const
{
    return _source.slice(_startMemeryLocationOfToken, _sourceIterator.getPosition()).toString();
}

Scanner::Character
Scanner::getCharacter() const
{
    return static_cast<Character>(*_sourceIterator);
}

void
Scanner::increment()
{
    ++_sourceIterator;
    ++_position;
    _column++;
}

bool
Scanner::isIdentifierStart(Scanner::Character character) const
{
    return (character >= Character::A && character <= Character::Z) ||
           (character >= Character::a && character <= Character::z) ||
           character == Character::_;
}


bool
Scanner::isIdentifierPart(Scanner::Character character) const
{
    return (character >= Character::a && character <= Character::z) ||
           (character >= Character::A && character <= Character::Z) ||
           (character >= Character::_0 && character <= Character::_9) ||
           character == Character::_;
}

Token
Scanner::scanExpected(Token token) {
    Token result = takeNextToken();
    if (result == token)
    {
        return token;
    }
    throw UnexpectedToken("Unexpected token ", result);
}
void
Scanner::saveCurrentLocation()
{
    _savedLocations.emplace(
            _position,
            _line,
            _column,
            _sourceIterator.value,
            _startMemeryLocationOfToken
    );
}

Token
Scanner::peekNextToken()
{
    saveCurrentLocation();
    Token token = takeNextToken();
    revertToSavedLocation();
    return token;
}

void
Scanner::revertToSavedLocation()
{
    SavedLocation location = _savedLocations.top();
    _line = location.line;
    _column = location.column;
    _position = location.position;
    _sourceIterator.value = location.positionPointer;
    _startMemeryLocationOfToken = location.startMemoryLocationOfToken;
}


char*
Scanner::getPosition() const
{
    return const_cast<char *>(_sourceIterator.value);
}


char*
Scanner::getStartPosition() const
{
    return const_cast<char *>(_startMemeryLocationOfToken);
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
