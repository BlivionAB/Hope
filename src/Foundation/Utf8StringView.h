#ifndef ELET_UTF8_STRING_VIEW_H
#define ELET_UTF8_STRING_VIEW_H

#include "Utf8String.h"

namespace elet::foundation
{

class Utf8StringView
{
public:

    Utf8StringView();

    Utf8StringView(const Utf8String& string);

    Utf8StringView(const char* value, const char* end);

    Utf8StringView(const char* string);

    class Iterator
    {
    public:

        explicit
        Iterator(const char* value);

        Utf8String::Character
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

        void
        setPosition(const char* position);

        Utf8String::Character
        peekNextChar();

        bool
        isEnd();

        const char*
        value;
    };

    Utf8StringView
    slice(const char* start, const char* end) const;

    Utf8StringView
    subString(size_t index) const;

    Utf8StringView
    subString(size_t start, size_t end) const;

    Utf8StringView
    slice(const char* start) const;

    [[nodiscard]] Iterator
    begin() const;

    [[nodiscard]] Iterator
    end() const;

    [[nodiscard]] const char*
    source() const;

    [[nodiscard]] const char*
    toString() const;

    [[nodiscard]] std::size_t
    size() const;

    unsigned char
    operator [] (std::size_t memoryIndex) const;

    bool
    operator == (const char* text) const;

    bool
    operator == (const Utf8StringView& other) const;

    bool
    operator < (const Utf8StringView& other) const;

    void
    operator = (const Utf8StringView& other);

    bool
    startsWith(Utf8StringView string);

private:

    const char*
    _value;

    const char*
    _end;
};

void
operator += (Utf8String& string1, const Utf8StringView& string2);

bool
operator == (const char* string1, const Utf8StringView& string2);

}
#endif