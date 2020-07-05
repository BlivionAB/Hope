#ifndef ELET_SYNTAX_H
#define ELET_SYNTAX_H

#include <cstddef>
#include <Foundation/Memory/Utf8Span.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>

struct ParameterList;
struct Punctuation;

enum class SyntaxKind
{
    Unknown,
    FunctionDeclaration,
    EnumDeclaration,
    FunctionBody,
    ModuleDeclaration,
    ModuleAccessUsage,
    ObjectDeclaration,
    InterfaceDeclaration,
    PropertyDeclaration,
    ImportStatement,
    ExportStatement,
    UseStatement,
    EndStatement,
    ParameterList,
    PropertyExpression,
    PropertyAccessExpression,
    Parameter,
    CallExpression,
    ArgumentList,
    ModuleSpecification,
    StringLiteral,
    ModuleScope,
    NamedUsage,
    WildcardUsage,
    Punctuation,
    Identifier,
};

struct Syntax
{
    SyntaxKind
    kind;

    char*
    start;

    char*
    end;
};


struct Identifier : Syntax
{
    Utf8StringView
    name;
};


struct FunctionBody : Syntax
{
    List<Syntax*>
    statements;
};


enum class TypeKind
{
    Int,
    UnsignedInt,
    Float,
    Double,
    Void,
    Custom,
};


struct Type : Syntax
{
    Identifier*
    name;

    List<Punctuation*>
    pointer;

    TypeKind
    type;
};


struct PropertyDeclaration : Syntax
{
    Identifier*
    name;

    ParameterList*
    parameters;

    Type*
    type;
};


struct InterfaceDeclaration : Syntax
{
    Identifier*
    name;

    List<PropertyDeclaration*>
    properties;
};


struct EnumDeclaration : Syntax
{
    Identifier*
    name;

    List<Identifier*>
    values;
};


struct Parameter : Syntax
{
    Identifier*
    name;

    Type*
    type;
};


struct ParameterList : Syntax
{
    List<Parameter*>
    parameters;
};


struct FunctionDeclaration : Syntax
{
    Identifier*
    name;

    ParameterList*
    parameterList;

    Type*
    type;

    FunctionBody*
    body;
};


struct EndStatement : Syntax
{
    Identifier*
    name;
};


struct ModuleSpecification : Syntax
{
    Identifier*
    name;
};


struct ModuleDeclaration : Syntax
{
    Identifier*
    name;

    List<Syntax*>
    declarations;

    EndStatement*
    endStatement;
};


struct ModuleScope : Syntax
{
    List<ModuleSpecification*>
    modules;
};

enum class PunctuationType
{
    Dot,
    Asterisk,
};

struct Punctuation : Syntax
{
    PunctuationType
    type;
};


struct Expression : Syntax
{

};


struct PropertyExpression : Expression
{
    Identifier*
    name;
};


struct PropertyAccessExpression : Expression
{
    Identifier*
    name;

    Punctuation*
    dot;

    Punctuation*
    question;

    Expression*
    expression;
};


struct BinaryOperator : Syntax
{

};

struct StringLiteral : Expression
{

};

struct BinaryExpression : Expression
{
    Expression*
    left;

    BinaryOperator*
    binaryOperator;

    Expression*
    right;
};

struct ImportStatement : Syntax
{
    StringLiteral*
    path;
};

struct Usage : Syntax
{

};


struct WildcardUsage : Usage
{

};


struct NamedUsage : Usage
{
    List<Identifier*>
    names;
};


struct ModuleAccessUsage : Usage
{
    Identifier*
    name;

    Usage*
    usage;
};


struct ExportStatement : Syntax
{
    ModuleAccessUsage*
    usage;
};


struct UseStatement : Syntax
{
    ModuleAccessUsage*
    usage;
};


struct ArgumentList : Syntax
{
    List<Expression*>
    arguments;
};


struct CallExpression : Expression
{
    Identifier*
    name;

    ArgumentList*
    argumentList;
};

#endif //ELET_SYNTAX_H
