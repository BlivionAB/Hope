#ifndef ELET_SYNTAX_H
#define ELET_SYNTAX_H

#include <cstddef>
#include <map>
#include <variant>
#include <Foundation/Memory/Utf8Span.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/Int128.h>
#include <Foundation/List.h>
#include <Domain/Compiler/CompilerTypes.h>
#include "Syntax.Labels.h"
#include "Syntax.Kind.h"


namespace elet::domain::compiler::instruction::output
{
    struct Instruction;
    struct Parameter;
    struct StoreRegisterInstruction;
}


namespace elet::domain::compiler::ast
{
    namespace type
    {
        struct Type;
    }
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



    struct Symbol
    {
        std::uint32_t
        index = 0;

        const
        Utf8StringView
        identifier;

        Utf8StringView
        name;

        Utf8StringView
        externalSymbol;

        std::uint32_t
        textOffset;

        Symbol(std::uint32_t textOffset, const Utf8StringView name):
            textOffset(textOffset),
            name(name)
        { }
    };


    enum class Token : std::size_t
    {
        Unknown,

        Identifier,
        Whitespace,

        // Keywords
        UsingKeyword,
        ImplementsKeyword,
        EnumKeyword,

        // Control Flows
        ReturnKeyword,
        IfKeyword,
        ElseKeyword,
        SwitchKeyword,
        CaseKeyword,
        WhenKeyword,
        ForKeyword,
        WhileKeyword,
        CancelKeyword,
        ObserveKeyword,
        OnKeyword,
        FromKeyword,

        // Values
        TrueKeyword,
        FalseKeyword,

        // Declarations
        AssemblyKeyword,
        FunctionKeyword,
        VarKeyword,
        DomainKeyword,
        ClassKeyword,
        InterfaceKeyword,
        ExternKeyword,

        // Punctuations
        OpenParen,
        CloseParen,
        OpenBrace,
        CloseBrace,
        OpenBracket,
        CloseBracket,
        Equal,
        EqualEqual,
        LessThan,
        GreaterThan,
        LessThanEqual,
        GreaterThanEqual,
        Comma,
        Colon,
        DoubleColon,
        SemiColon,
        Dot,
        DoubleQuote,
        SingleQuote,
        BackwardSlash,
        Hash,


        AmpersandAmpersand,
        PipePipe,

        // Operators
        Plus,
        Minus,
        ForwardSlash,
        Asterisk,
        Ampersand,
        Pipe,
        Caret,
        Percent,

        BinaryOperationStart = AmpersandAmpersand,
        BinaryOperationEnd = Percent,

        // Types
        VoidKeyword,
        BoolKeyword,
        IntKeyword,
        UnsignedIntKeyword,
        CharKeyword,
        StringKeyword,
        U8Keyword,
        U16Keyword,
        U32Keyword,
        U64Keyword,
        S8Keyword,
        S16Keyword,
        S32Keyword,
        S64Keyword,
        USizeKeyword,
        LiteralKeyword,

        // Modifiers
        ContextKeyword,
        TypesKeyword,
        ParamsKeyword,
        AsyncKeyword,
        PublicKeyword,
        PrivateKeyword,
        StaticKeyword,

        // Operators
        LengthOfKeyword,
        SizeOfKeyword,

        StringLiteral,
        CharacterLiteral,
        FloatLiteral,

        HexadecimalLiteral,
        DecimalLiteral,
        BinaryLiteral,

        EndOfFile,
    };


    struct SourceFile
    {
        const char*
        start;

        const char*
        end;

        fs::path
        file;

        NameToDeclarationMap
        declarations;

        std::multimap<Utf8StringView, Declaration*>
        symbols;

        SourceFile(fs::path file, const char* start, const char* end):
            file(file),
            start(start),
            end(end)
        { }
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


    struct EndOfFile : Syntax
    {

    };


    struct ErrorNode : Syntax
    {

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

        RegisterSize
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
        // Resolved tyoe is the required type needed to store the result of the expression.
        Type*
        resolvedType;

        // Operating type is the required type needed to execute the operation of the expression.
        Type*
        operatingType;
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


    struct CharacterLiteral : Expression
    {
        uint8_t
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


    struct IntegerSuffix : Syntax
    {
        Token
        type;
    };


    struct BinaryLiteral : Expression
    {

    };


    struct DecimalLiteral : Expression
    {

    };


    struct HexadecimalLiteral : Expression
    {

    };


    struct IntegerLiteral : Expression
    {
        std::variant<HexadecimalLiteral*, DecimalLiteral*, BinaryLiteral*>
        digits;

        Int128
        value;

        IntegerSuffix*
        suffix;

        bool
        isNegative = false;

        int
        underscoresCount = 0;
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
        Add,
        Minus,
        Multiply,
        Divide,
        Modulo,

        BitwiseAnd,
        BitwiseXor,
        BitwiseOr,
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

        uint8_t
        operatorPrecedence;
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


#include "Syntax.Type.h"
#include "Syntax.Declarations.h"
#include "Syntax.Block.h"
#include "Syntax.Statement.h"
#include "Domain/Compiler/Instruction/Instruction.h"


#endif //ELET_SYNTAX_H
