#include "TextScanner.h"

namespace elet::foundation
{
    TextScanner::TextScanner(const Utf8StringView source) :
        _source(source),
        _sourceIterator(source.begin()),
        _endIterator(source.end())
    {

    }


    void
    TextScanner::setTokenStartPosition()
    {
        _startMemoryLocationOfToken = _sourceIterator.getPositionAddress();
    }


    TextScanner::Character
    TextScanner::getCharacter() const
    {
        return static_cast<Character>(*_sourceIterator);
    }

    void
    TextScanner::increment()
    {
        ++_sourceIterator;
        Character character = getCharacter();
        ++_position;
        if (isNewline(character))
        {
            _column = 1;
            _line++;
            return;
        }
        _column++;
    }


    Utf8StringView
    TextScanner::getTokenValue() const
    {
        return _source.slice(_startMemoryLocationOfToken, _sourceIterator.getPositionAddress()).toString();
    }


    bool
    TextScanner::isIdentifierStart(TextScanner::Character character) const
    {
        return (character >= Character::A && character <= Character::Z) ||
               (character >= Character::a && character <= Character::z) ||
               character == Character::_;
    }


    bool
    TextScanner::isIdentifierPart(TextScanner::Character character) const
    {
        return (character >= Character::a && character <= Character::z) ||
               (character >= Character::A && character <= Character::Z) ||
               (character >= Character::_0 && character <= Character::_9) ||
               character == Character::_;
    }


    size_t
    TextScanner::getPosition() const
    {
        return _position;
    }


    char*
    TextScanner::getPositionAddress() const
    {
        return const_cast<char *>(_sourceIterator.value);
    }


    char*
    TextScanner::getStartPosition() const
    {
        return const_cast<char *>(_startMemoryLocationOfToken);
    }


    void
    TextScanner::saveCurrentLocation()
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
    TextScanner::getSource()
    {
        return _source;
    }


    void
    TextScanner::revertToSavedLocation()
    {
        Location location = _savedLocations.top();
        seek(location);
    }


    void
    TextScanner::seek(const Location &location)
    {
        _line = location.line;
        _column = location.column;
        _position = location.position;
        _sourceIterator.value = location.positionPointer;
        _startMemoryLocationOfToken = location.startMemoryLocationOfToken;
    }


    TextScanner::Location
    TextScanner::getLocation() const
    {
        return TextScanner::Location(
            _position,
            _line,
            _column,
            _sourceIterator.value,
            _startMemoryLocationOfToken
        );
    }


    bool
    TextScanner::isDigit(Character character) const
    {
        return character >= Character::_0 && character <= Character::_9;
    }


    bool
    TextScanner::isHexDigit(Character character) const
    {
        return character >= Character::_0 && character <= Character::_9 || character >= Character::a && character <= Character::f;
    }


    void
    TextScanner::scanRestOfLine()
    {
        while (!isNewline(getCharacter()))
        {
            increment();
        }
    }


    void
    TextScanner::scanToNextSemicolon()
    {
        while (getCharacter() != Character::Semicolon)
        {
            increment();
        }
        increment();
    }


    bool
    TextScanner::isNewline(TextScanner::Character character) const
    {
        return character == Character::Newline || character == Character::CarriageReturn;
    }


    std::size_t
    TextScanner::getDecimalValue() const
    {
        Utf8StringView token = getTokenValue();
        std::size_t lastIndex = token.size() - 1;
        const char* digits = token.source();
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
    TextScanner::getHexadecimalValue() const
    {
        Utf8StringView token = getTokenValue();
        std::size_t lastIndex = token.size() - 1;
        const char* digits = token.source();
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


    void
    TextScanner::scanToPositionAddress(const char* positionAddress)
    {
        while (getPositionAddress() != positionAddress)
        {
            increment();
        }
    }
}