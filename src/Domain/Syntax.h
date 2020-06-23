#ifndef ELET_SYNTAX_H
#define ELET_SYNTAX_H

#include <cstddef>
#include <Foundation/Memory/Utf8Span.h>
#include <Foundation/Utf8StringView.h>

enum class SyntaxKind
{
    FunctionDeclaration,
};

struct Syntax
{
    SyntaxKind
    kind;

    std::size_t
    start;

    std::size_t
    end;
};

struct FunctionBody : Syntax
{
    std::vector<Syntax*>
    statements;
};

struct FunctionDeclaration : Syntax
{
    Utf8StringView
    name;

    FunctionBody*
    body;
};

#endif //ELET_SYNTAX_H
