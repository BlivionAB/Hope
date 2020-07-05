#ifndef ELET_SCANNER_H
#define ELET_SCANNER_H

#include <Foundation/Utf8StringView.h>
#include <Foundation/HashTableMap.h>
#include <stack>

using namespace elet::foundation;


class Scanner
{
public:
    explicit Scanner(const Utf8StringView& source);

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

    enum class Token : std::size_t
    {
        Unknown,

        Identifier,

        // Keywords
        ImportKeyword,
        ExportKeyword,
        UseKeyword,
        ObjectKeyword,
        InterfaceKeyword,
        ModuleKeyword,
        EndKeyword,
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
        Comma,
        Colon,
        DoubleColon,
        SemiColon,
        Asterisk,
        Dot,
        DoubleQuote,
        SingleQuote,

        // Types
        VoidKeyword,
        IntKeyword,
        UnsignedIntKeyword,
        FloatKeyword,
        DoubleKeyword,

        StringLiteral,

        EndOfFile,
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
        startMemoryLocationOfToken;

        const char*
        positionPointer;

        SavedLocation(
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

    char*
    getPosition() const;

    char*
    getStartPosition() const;

    void
    scan(const Utf8StringView& source);

private:

    std::size_t
    _position = 0;

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
    _startMemeryLocationOfToken = nullptr;

    std::stack<SavedLocation>
    _savedLocations;

    std::size_t
    _startLineOfToken = 1;

    std::size_t
    _startColumnOfToken = 1;

    void
    increment();

    void
    setTokenStartPosition();

    Character
    getCharacter() const;

    Token
    scanString();

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

using Token = Scanner::Token;
const HashTableMap<const char*, Token> eletStringToToken =
{
    { "enum", Token::EnumKeyword },
    { "interface", Token::InterfaceKeyword },
    { "module", Token::ModuleKeyword },
    { "import", Token::ImportKeyword },
    { "export", Token::ExportKeyword },
    { "use", Token::UseKeyword },
    { "implement", Token::ImplementKeyword },
    { "module", Token::ModuleKeyword },
    { "end", Token::EndKeyword },
    { "object", Token::ObjectKeyword },
    { "fn", Token::FunctionKeyword },
    { "if", Token::IfKeyword },
    { "else", Token::ElseKeyword },
    { "return", Token::ReturnKeyword },
    { "enum", Token::EnumKeyword },
    { "int", Token::IntKeyword },
    { "uint", Token::UnsignedIntKeyword },
    { "void", Token::VoidKeyword },
};

const HashTableMap<Token, const char*> eletTokenToString =
{
    { Token::EnumKeyword, "enum" },
    { Token::InterfaceKeyword, "interface" },
    { Token::ModuleKeyword, "module" },
    { Token::ImportKeyword, "import" },
    { Token::ExportKeyword, "export" },
    { Token::UseKeyword, "use" },
    { Token::ImplementKeyword, "implement" },
    { Token::ModuleKeyword, "module" },
    { Token::EndKeyword , "end" },
    { Token::ObjectKeyword, "object" },
    { Token::FunctionKeyword, "fn" },
    { Token::IfKeyword, "if" },
    { Token::ElseKeyword, "else" },
    { Token::ReturnKeyword, "return" },
    { Token::EnumKeyword, "enum" },
    { Token::IntKeyword, "int" },
    { Token::UnsignedIntKeyword, "uint" },
    { Token::VoidKeyword, "void" },

    { Token::Identifier, "identifier" },
    { Token::DoubleColon, "::" },
    { Token::SemiColon, ";" },
};


#endif //ELET_SCANNER_H
