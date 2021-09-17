#ifndef ELET_SYNTAX_H
#define ELET_SYNTAX_H

#include <cstddef>
#include <map>
#include <Foundation/Memory/Utf8Span.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include "Syntax.Labels.h"
#include "Syntax.Kind.h"
#include "Syntax.Type.h"


namespace elet::domain::compiler
{
    struct Symbol;
}


namespace elet::domain::compiler::instruction::output
{
    struct Instruction;
    struct Parameter;
    struct StoreRegisterInstruction;
}


namespace elet::domain::compiler::ast
{
    using namespace instruction;
    using namespace type;

    struct ParameterDeclaration;
    struct DomainDeclaration;
    struct ParameterDeclarationList;
    struct Punctuation;
    struct Expression;
    struct Declaration;
    struct CallExpression;
    using NameToDeclarationMap = std::map<Utf8StringView, Declaration*>;


    struct SourceFile
    {
        NameToDeclarationMap
        declarations;

        std::multimap<Utf8StringView, Declaration*>
        symbols;
    };



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


    struct Name : Syntax
    {
        Utf8StringView
        name;
    };


    struct MetadataProperty : Syntax
    {
        Name*
        name;

        Expression*
        value;
    };

    struct DeclarationMetadata : Syntax
    {
        List<MetadataProperty*>
        properties;
    };



    struct TypeAssignment : Syntax
    {
        Name*
        name;

        List<Punctuation*>
        pointers;

        TypeKind
        type;

        type::TypeSize
        size;

        Name*
        parameter;

        Utf8String
        display;

        bool
        isLiteral = false;
    };


    struct TypeDeclaration : Syntax
    {
        Name*
        name;
    };


    enum class DomainType
    {
        Unknown,
        Start,
        End,
    };


    struct Modifier : Syntax
    {

    };


    struct EndStatement : Syntax
    {
        Name*
        name;
    };


    struct ModuleSpecification : Syntax
    {
        Name*
        name;
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
        Type*
        resolvedType;
    };


    struct NamedExpression : Expression
    {
        Name*
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
        Name*
        name;

        Expression*
        expression;
    };


    struct AddressOfExpression : Expression
    {
        Expression*
        expression;
    };


    struct BinaryOperator : Syntax
    {

    };


    struct BooleanLiteral : Expression
    {
        bool
        value;
    };


    struct StringLiteral : Expression
    {
        const char*
        stringStart;

        const char*
        stringEnd;

        Type*
        resolvedType;
    };


    struct IntegerLiteral : Expression
    {
        int64_t
        value;
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


    enum class BinaryOperatorKind
    {
        And,
        Or,
        Equal,
        Plus,
    };


    struct BinaryExpression : Expression
    {
        Expression*
        left;

        BinaryOperator*
        binaryOperator;

        BinaryOperatorKind
        binaryOperatorKind;

        Expression*
        right;
    };


    struct UsageClause : Syntax
    {
        List<Name*>
        names;
    };


    struct UsingStatement : Syntax
    {
        List<Name*>
        domains;

        UsageClause*
        usageClause;

        DomainDeclaration*
        domain;
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
}


#include "Syntax.Declarations.h"
#include "Syntax.Block.h"
#include "Domain/Compiler/Instruction/Instruction.h"


#endif //ELET_SYNTAX_H
