#include "Scanner.h"
#include "Foundation/HashTableMap.h"
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
        Character character = getCharacter();
        increment();
        switch (character)
        {
            case Character::CarriageReturn:
            case Character::Newline:
            case Character::Space:
                setTokenStartPosition();
                continue;
            case Character::OpenParen:
                return Token::OpenParen;
            case Character::CloseParen:
                return Token::CloseParen;
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
}

using Token = Scanner::Token;
const HashTableMap<const char*, Token, 18> eletTokenToString =
{
    { "object", Token::ObjectKeyword },
    { "fn", Token::FunctionKeyword },
    { "if", Token::IfKeyword },
    { "else", Token::ElseKeyword },
    { "return", Token::ReturnKeyword },
    { "enum", Token::EnumKeyword },
};

Token
Scanner::getTokenFromString(const Utf8StringView& string) const
{
    std::optional token = eletTokenToString.find(string);
    if (token)
    {
        return *token;
    }
    return Token::Identifier;
}


void
Scanner::setTokenStartPosition()
{
    _startPositionOfToken = _sourceIterator.getPosition();
    _startColumnOfToken = _column;
    _startLineOfToken = _line;
}

Utf8StringView
Scanner::getTokenValue() const
{
    return _source.slice(_startPositionOfToken, _sourceIterator.getPosition()).toString();
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
        _sourceIterator.value
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
    _sourceIterator.value = location.pointer;
}

std::size_t
Scanner::getPosition()
{
    return _position;
}
