#ifndef ELET_SYNTAX_H
#define ELET_SYNTAX_H

#include <cstddef>
#include <map>
#include <Foundation/Memory/Utf8Span.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include "Instruction/Instruction.h"


namespace elet::domain::compiler::instruction
{
    namespace output
    {
        struct Instruction;
    }
}

namespace elet::domain::compiler::ast
{


using namespace instruction;

struct ParameterDeclarationList;
struct Punctuation;
struct Expression;
struct Declaration;
using NameToDeclarationMap = std::map<Utf8StringView, Declaration*>;

struct File
{
    NameToDeclarationMap
    declarations;
};

enum class SyntaxKind : std::uint8_t
{
    Unknown,
    AssemblyBlock,
    AssemblyBody,
    ArrayLiteral,
    MetadataProperty,
    DeclarationMetadata,
    FunctionDeclaration,
    EnumDeclaration,
    FunctionBody,
    ModuleDeclaration,
    ModuleAccessUsage,
    ModuleAccessExpression,
    PropertyDeclaration,
    ImportStatement,
    ExportStatement,
    UseStatement,
    EndStatement,
    ParameterDeclarationList,
    PropertyExpression,
    PropertyAccessExpression,
    ParameterDeclaration,
    Type,
    CallExpression,
    ArgumentList,
    ModuleSpecification,
    StringLiteral,
    Tuple,
    ModuleScope,
    NamedUsage,
    WildcardUsage,
    Punctuation,
    Identifier,

    // Expressions
    LengthOfExpression,

    // Declarations
    ObjectDeclaration,
    InterfaceDeclaration,
    VariableDeclaration,

};


#define DECLARATION_LABEL       1 << 0
#define BLOCK_LABEL             1 << 1
#define NAMED_EXPRESSION_LABEL  1 << 2


struct Syntax
{
    SyntaxKind
    kind;

    std::uint8_t
    labels;

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

    NameToDeclarationMap
    symbols;
};


struct AssemblyBody : Syntax
{
    List<output::Instruction*>*
    instructions;
};


struct MetadataProperty : Syntax
{
    Identifier*
    name;

    Expression*
    value;
};

struct DeclarationMetadata : Syntax
{
    List<MetadataProperty*>
    properties;
};


struct AssemblyBlock : Syntax
{
    DeclarationMetadata*
    metadata;

    AssemblyBody*
    body;
};


enum class TypeKind
{
    Int,
    Char,
    UInt,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float,
    Double,
    Void,
    Custom,
    Length,
};


struct Type : Syntax
{
    Identifier*
    name;

    List<Punctuation*>
    pointers;

    TypeKind
    type;

    std::size_t
    size;

    Identifier*
    parameter;

    Utf8String
    display;
};


struct Declaration : Syntax
{
    Identifier*
    name;

    std::size_t
    offset;

    Utf8String
    symbol;
};


struct PropertyDeclaration : Declaration
{
    ParameterDeclarationList*
    parameters;

    Type*
    type;
};


struct InterfaceDeclaration : Declaration
{
    List<PropertyDeclaration*>
    properties;
};


struct EnumDeclaration : Declaration
{
    List<Identifier*>
    values;
};


struct ParameterDeclaration : Declaration
{
    Type*
    type;

    Utf8String
    display;
};


struct ParameterDeclarationList : Syntax
{
    List<ParameterDeclaration*>
    parameters;
};


struct FunctionDeclaration : Declaration
{
    ParameterDeclarationList*
    parameterList;

    Type*
    type;

    FunctionBody*
    body;
};


struct VariableDeclaration : Declaration
{
    Type*
    type;

    Punctuation*
    equals;

    Expression*
    expression;
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


struct ModuleDeclaration : Declaration
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


struct NamedExpression : Expression
{
    Identifier*
    name;

    Declaration*
    referenceDeclaration;
};


struct PropertyExpression : NamedExpression
{

};


struct PropertyAccessExpression : NamedExpression
{
    Punctuation*
    dot;

    Punctuation*
    question;

    Expression*
    expression;
};


struct ModuleAccessExpression : Expression
{
    Identifier*
    name;

    Expression*
    expression;
};


struct BinaryOperator : Syntax
{

};


struct StringLiteral : Expression
{
    const char*
    stringStart;

    const char*
    stringEnd;
};


struct ArrayLiteral : Expression
{
    List<Expression*>
    values;
};


struct Tuple : Expression
{
    List<Expression*>
    values;
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


struct CallExpression : NamedExpression
{
    ArgumentList*
    argumentList;
};


struct LengthOfExpression : Expression
{
    Identifier*
    reference;
};


}

#endif //ELET_SYNTAX_H
