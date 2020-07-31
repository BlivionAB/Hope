#include "BaseScanner.h"

namespace elet::foundation
{

BaseScanner::BaseScanner(const Utf8StringView &source) :
    _source(source),
    _sourceIterator(source.begin()),
    _endIterator(source.end())
{

}


void
BaseScanner::setTokenStartPosition()
{
    _startMemoryLocationOfToken = _sourceIterator.getPosition();
}


BaseScanner::Character
BaseScanner::getCharacter() const
{
    return static_cast<Character>(*_sourceIterator);
}

void
BaseScanner::increment() {
    ++_sourceIterator;
    ++_position;
    _column++;
}


Utf8StringView
BaseScanner::getTokenValue() const
{
    return _source.slice(_startMemoryLocationOfToken, _sourceIterator.getPosition()).toString();
}


bool
BaseScanner::isIdentifierStart(BaseScanner::Character character) const
{
    return (character >= Character::A && character <= Character::Z) ||
           (character >= Character::a && character <= Character::z) ||
           character == Character::_;
}


bool
BaseScanner::isIdentifierPart(BaseScanner::Character character) const
{
    return (character >= Character::a && character <= Character::z) ||
           (character >= Character::A && character <= Character::Z) ||
           (character >= Character::_0 && character <= Character::_9) ||
           character == Character::_;
}


char*
BaseScanner::getPosition() const
{
    return const_cast<char *>(_sourceIterator.value);
}


char*
BaseScanner::getStartPosition() const
{
    return const_cast<char *>(_startMemoryLocationOfToken);
}


void
BaseScanner::saveCurrentLocation()
{
    _savedLocations.emplace(
        _position,
        _line,
        _column,
        _sourceIterator.value,
        _startMemoryLocationOfToken
    );
}


Utf8StringView&
BaseScanner::getSource()
{
    return _source;
}


void
BaseScanner::revertToSavedLocation()
{
    Location location = _savedLocations.top();
    seek(location);
}


void
BaseScanner::seek(const Location &location)
{
    _line = location.line;
    _column = location.column;
    _position = location.position;
    _sourceIterator.value = location.positionPointer;
    _startMemoryLocationOfToken = location.startMemoryLocationOfToken;
}


BaseScanner::Location
BaseScanner::getLocation() const
{
    return BaseScanner::Location(
        _position,
        _line,
        _column,
        _sourceIterator.value,
        _startMemoryLocationOfToken
    );
}


bool
BaseScanner::isDigit(Character character) const
{
    return character >= Character::_0 && character <= Character::_9;
}


bool
BaseScanner::isHexDigit(Character character) const
{
    return character >= Character::_0 && character <= Character::_9 || character >= Character::a && character <= Character::f;
}


void
BaseScanner::scanRestOfLine()
{
    while (!isNewline(getCharacter()))
    {
        increment();
    }
}


bool
BaseScanner::isNewline(BaseScanner::Character character) const
{
    return character == Character::Newline || character == Character::CarriageReturn;
}


std::size_t
BaseScanner::getDecimalValue() const
{
    Utf8StringView token = getTokenValue();
    std::size_t lastIndex = token.size() - 1;
    const char* digits = token.asString();
    unsigned exponent = 1;
    std::size_t result = 0;
    for (int i = lastIndex; i >= 0; i--)
    {
        std::size_t digit = static_cast<std::size_t>(digits[i]) - static_cast<std::size_t>(Utf8String::Character::_0);
        result += digit * exponent;
        exponent *= 10;
    }
    return result;
}


std::size_t
BaseScanner::getHexadecimalValue() const
{
    Utf8StringView token = getTokenValue();
    std::size_t lastIndex = token.size() - 1;
    const char* digits = token.asString();
    unsigned int exponent = 1;
    std::size_t result = 0;
    for (int i = lastIndex; i >= 2; i--)
    {
        std::size_t digit;
        Utf8String::Character digitCharacter = static_cast<Utf8String::Character>(digits[i]);
        if (isDigit(digitCharacter))
        {
            digit = static_cast<std::size_t>(digitCharacter) - static_cast<std::size_t>(Utf8String::Character::_0);
        }
        else
        {
            digit = static_cast<std::size_t>(digitCharacter) - static_cast<std::size_t>(Utf8String::Character::W);
        }
        result += digit * exponent;
        exponent *= 16;
    }
    return result;
}

}