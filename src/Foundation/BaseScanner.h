#ifndef ELET_BASESCANNER_H
#define ELET_BASESCANNER_H
#include <cstddef>
#include <Foundation/Utf8String.h>
#include <Foundation/Utf8StringView.h>
#include <stack>

namespace elet::foundation
{

class BaseScanner
{
public:
    explicit BaseScanner(const Utf8StringView source);


    struct Location
    {
        std::size_t
        position;

        std::size_t
        line;

        std::size_t
        column;

        const char*
        startMemoryLocationOfToken;

        const char*
        positionPointer;

        Location(
            std::size_t
            position,

            std::size_t
            line,

            std::size_t
            column,

            const char*
            pointerPosition,

            const char*
            startPointerPositionOfToken
        ):
            position(position),
            line(line),
            column(column),
            positionPointer(pointerPosition),
            startMemoryLocationOfToken(startPointerPositionOfToken)
        { }
    };

    using Character = Utf8String::Character;

    size_t
    getPosition() const;

    char*
    getPositionAddress() const;

    char*
    getStartPosition() const;

    Location
    getLocation() const;

    void
    saveCurrentLocation();

    void
    revertToSavedLocation();

    Utf8StringView&
    getSource();

    void
    seek(const Location& location);

    Utf8StringView
    getTokenValue() const;

    bool
    isDigit(Character character) const;

    bool
    isHexDigit(Character character) const;

    std::size_t
    getDecimalValue() const;

    std::size_t
    getHexadecimalValue() const;

    void
    scanToNextSemicolon();

    void
    scanRestOfLine();

    Character
    getCharacter() const;

    void
    increment();

    bool
    isNewline(Character character) const;

protected:

    std::stack<Location>
    _savedLocations;

    size_t
    _position = 0;

    size_t
    _line = 1;

    size_t
    _column = 1;

    Utf8StringView
    _source;

    Utf8StringView::Iterator
    _sourceIterator;

    Utf8StringView::Iterator
    _endIterator;

    const char*
    _startMemoryLocationOfToken = nullptr;

    void
    setTokenStartPosition();

    bool
    isIdentifierStart(Character character) const;

    bool
    isIdentifierPart(Character character) const;
};

}

#endif //ELET_X86_SCANNER_H
