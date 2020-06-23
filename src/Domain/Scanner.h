#ifndef ELET_SCANNER_H
#define ELET_SCANNER_H

#include <Foundation/Utf8StringView.h>
#include <stack>

using namespace elet::foundation;

class Scanner
{
public:
    Scanner(const Utf8StringView& source);

    using Character = Utf8String::Character;

    struct SourceLocation
    {
        std::size_t
        position;

        std::size_t
        line;

        std::size_t
        column;

        SourceLocation(
            std::size_t
            position,

            std::size_t
            line,

            std::size_t
            column):

            position(position),
            line(line),
            column(column)
        { }
    };

    enum class Token
    {
        Unknown,

        Identifier,

        // Keywords
        ObjectKeyword,
        ImplementKeyword,
        FunctionKeyword,
        EnumKeyword,
        ReturnKeyword,
        IfKeyword,
        ElseKeyword,
        WhenKeyword,
        ForKeyword,
        WhileKeyword,
        CancelKeyword,
        ObserveKeyword,
        OnKeyword,
        FromKeyword,

        OpenParen,
        CloseParen,
        OpenBrace,
        CloseBrace,
        OpenBracket,
        CloseBracket,
        LessThan,
        GreaterThan,
        LessThanEqual,
        GreaterThanEqual,

        OneOfFile,
    };

    Token
    peekNextToken();

    Token
    takeNextToken();

    Token
    scanExpected(Token token);

    Utf8StringView
    getTokenValue() const;

    struct SavedLocation
    {
        std::size_t
        position;

        std::size_t
        line;

        std::size_t
        column;

        const char*
        pointer;

        SavedLocation(
            std::size_t
            position,

            std::size_t
            line,

            std::size_t
            column,

            const char*
            pointer
        ):
            position(position),
            line(line),
            column(column),
            pointer(pointer)
        { }

    };

    std::size_t
    getPosition();

private:

    std::size_t
    _position;

    std::size_t
    _line = 1;

    std::size_t
    _column = 1;

    Utf8StringView
    _source;

    Utf8StringView::Iterator
    _sourceIterator;

    Utf8StringView::Iterator
    _endIterator;

    const char*
    _startPositionOfToken = nullptr;

    std::stack<SavedLocation>
    _savedLocations;

    std::size_t
    _startLineOfToken = 1;

    std::size_t
    _startColumnOfToken = 1;

    void
    setTokenStartPosition();

    void
    increment();

    Character
    getCharacter() const;

    bool
    isIdentifierStart(Character character) const;

    bool
    isIdentifierPart(Character character) const;

    Token
    getTokenFromString(const Utf8StringView& string) const;

    void
    saveCurrentLocation();

    void
    revertToSavedLocation();
};

#endif //ELET_SCANNER_H
