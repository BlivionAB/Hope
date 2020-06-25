#ifndef FLASHPOINT_UTF8_STRING_H
#define FLASHPOINT_UTF8_STRING_H

#include <string>

#define UTF8_EOF 0x04

class Utf8String
{
public:

    enum class Character
    {
        NullCharacter = 0x00,
        Eof = 0x04,

        _ = 0x5F,
        $ = 0x24,

        _0 = 0x30,
        _1 = 0x31,
        _2 = 0x32,
        _3 = 0x33,
        _4 = 0x34,
        _5 = 0x35,
        _6 = 0x36,
        _7 = 0x37,
        _8 = 0x38,
        _9 = 0x39,

        a = 0x61,
        b = 0x62,
        c = 0x63,
        d = 0x64,
        e = 0x65,
        f = 0x66,
        g = 0x67,
        h = 0x68,
        i = 0x69,
        j = 0x6A,
        k = 0x6B,
        l = 0x6C,
        m = 0x6D,
        n = 0x6E,
        o = 0x6F,
        p = 0x70,
        q = 0x71,
        r = 0x72,
        s = 0x73,
        t = 0x74,
        u = 0x75,
        v = 0x76,
        w = 0x77,
        x = 0x78,
        y = 0x79,
        z = 0x7A,

        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4A,
        K = 0x4B,
        L = 0x4C,
        M = 0x4D,
        N = 0x4E,
        O = 0x4F,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5a,

        // Unicode 3.0 space characters
        Space = 0x20,
        NonBreakingSpace = 0x00A0,
        EnQuad = 0x2000,
        EmQuad = 0x2001,
        EnSpace = 0x2002,
        EmSpace = 0x2003,
        ThreePerEmSpace = 0x2004,
        FourPerEmSpace = 0x2005,
        SixPerEmSpace = 0x2006,
        FigureSpace = 0x2007,
        PunctuationSpace = 0x2008,
        ThinSpace = 0x2009,
        HairSpace = 0x200A,
        ZeroWidthSpace = 0x200B,
        NarrowNoBreakSpace = 0x202F,
        IdeoGraphicSpace = 0x3000,
        MathematicalSpace = 0x205F,
        Ogham = 0x1680,
        HorizontalTab = 0x09,

        Newline = 0x0A,               // \n
        CarriageReturn = 0x0D,        // \r
        LineSeparator = 0x2028,
        ParagraphSeparator = 0x2029,
        NextLine = 0x0085,

        Backspace = 0x08,             // \b
        FormFeed = 0x0C,              // \f
        Tab = 0x09,                   // \t
        VerticalTab = 0x0B,           // \v
        ByteOrderMark = 0xFEFF,

        Ampersand = 0x26,             // &
        Asterisk = 0x2A,              // *
        At = 0x40,                    // @
        Backslash = 0x5C,             // \
        Backtick = 0x60,              // `
        Pipe = 0x7C,                   // |
        Caret = 0x5E,                 // ^
        CloseBrace = 0x7D,            // }
        CloseBracket = 0x5D,          // ]
        CloseParen = 0x29,            // )
        Colon = 0x3A,                 // :
        Comma = 0x2C,                 // ,
        Dot = 0x2E,                   // .
        DoubleQuote = 0x22,           // "
        Equal = 0x3D,                 // =
        Exclamation = 0x21,           // !
        Dollar = 0x24,                // $
        GreaterThan = 0x3E,           // >
        Hash = 0x23,                  // #
        LessThan = 0x3C,              // <
        Minus = 0x2D,                 // -
        Dash = Minus,                 // -
        OpenBrace = 0x7B,             // {
        OpenBracket = 0x5B,           // [
        OpenParen = 0x28,             // (
        Percent = 0x25,               // %
        Plus = 0x2B,                  // +
        Question = 0x3F,              // ?
        Semicolon = 0x3B,             // ;
        SingleQuote = 0x27,           // '
        Slash = 0x2F,                 // /
        Tilde = 0x7E,                 // ~
        Underscore = 0x5F,            // _

        MaxAsciiCharacter = 0x7F,     // DEL

        End = 0xFFFF,
    };


    Utf8String();

    Utf8String(const char* text);

    Utf8String(const char* text, std::size_t size);

    Utf8String(const Utf8String& other);

    ~Utf8String();

    void
    reserve(std::size_t size);

    bool
    operator != (const Utf8String& target) const;

    bool
    operator == (const Utf8String& target) const;

    bool
    operator == (const char* target) const;

    void
    operator += (const char* text);

    void
    operator += (const Utf8String& other);

    void
    operator += (char ch);

    bool
    operator < (const Utf8String& string) const;

    Utf8String
    operator + (const Utf8String& string) const;

    Utf8String&
    operator = (const Utf8String& other);

    friend
    std::ostream&
    operator << (std::ostream& output, const Utf8String& string);

    char
    operator [] (std::size_t memoryIndex) const;

    friend
    Utf8String
    operator + (const char* text, const Utf8String&);

    class Iterator
    {
    public:

        explicit
        Iterator(const char* _value);

        Character
        operator * () const;

        void
        operator += (std::size_t offset);

        bool
        operator == (const Iterator& other) const;

        bool
        operator != (const Iterator& other) const;

        void
        operator ++ ();

        const char*
        getPosition() const;

    private:

        const char*
        _value;
    };

    Iterator
    begin() const;

    Iterator
    end() const;

    std::size_t
    size() const;

    const char*
    toString() const;

    const char*
    asString() const;

    void
    set(const char*, std::size_t);

    void
    append(const char*, std::size_t);

private:

    char*
    _value;

    char*
    _end;

    std::size_t
    _capacity;
};
