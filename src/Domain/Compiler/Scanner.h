#ifndef ELET_SOME_SCANNER_H
#define ELET_SOME_SCANNER_H

#include <Foundation/Utf8StringView.h>
#include <Foundation/HashTableMap.h>
#include <Foundation/BaseScanner.h>
#include <stack>

using namespace elet::foundation;

#define TREAT_STRING_KEYWORD_AS_NAME (std::uint8_t)0x1

class Scanner : public elet::foundation::BaseScanner
{
public:
    explicit Scanner(const Utf8StringView& source);

    enum class Token : std::size_t
    {
        Unknown,

        Identifier,

        // Keywords
        UsingKeyword,
        ImplementsKeyword,
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

        Ampersand,
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
        Asterisk,
        Dot,
        DoubleQuote,
        SingleQuote,


        AmpersandAmpersand,
        PipePipe,
        Plus,
        Minus,

        BinaryOperationStart = AmpersandAmpersand,
        BinaryOperationEnd = Minus,

        // Types
        VoidKeyword,
        IntKeyword,
        UnsignedIntKeyword,
        CharKeyword,
        StringKeyword,
        F32Keyword,
        F64Keyword,
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
        FloatLiteral,
        IntegerLiteral,

        EndOfFile,
    };

    Token
    peekNextToken();

    Token
    takeNextToken();

    Utf8StringView
    getTokenValue() const;

    void
    setFlag(std::uint8_t stage);

    void
    resetFlags();

private:

    Token
    scanString();

    Token
    getTokenFromString(const Utf8StringView& string) const;

    std::uint8_t
    _stage;

    Token
    scanDigits();
};

using Token = Scanner::Token;
const HashTableMap<const char*, Token> eletStringToToken =
{
    // Declarations
    { "domain", Token::DomainKeyword },
    { "fn", Token::FunctionKeyword },
    { "class", Token::ClassKeyword },
    { "interface", Token::InterfaceKeyword },
    { "extern", Token::ExternKeyword },
    { "enum", Token::EnumKeyword },
    { "var", Token::VarKeyword },
    { "assembly", Token::AssemblyKeyword },

    // Modifiers / Metadata / Clauses
    { "static", Token::StaticKeyword },
    { "public", Token::PublicKeyword },
    { "private", Token::PrivateKeyword },
    { "types", Token::TypesKeyword },
    { "params", Token::ParamsKeyword },
    { "context", Token::ContextKeyword },
    { "implements", Token::ImplementsKeyword },
    { "from", Token::FromKeyword },

    // Control flows
    { "if", Token::IfKeyword },
    { "else", Token::ElseKeyword },
    { "for", Token::ForKeyword },
    { "while", Token::WhileKeyword },
    { "return", Token::ReturnKeyword },

    // Operators
    { "lengthof", Token::LengthOfKeyword },

    // Types
    { "int", Token::IntKeyword },
    { "uint", Token::UnsignedIntKeyword },
    { "usize", Token::USizeKeyword },
    { "char", Token::CharKeyword },
    { "string", Token::StringKeyword },
    { "void", Token::VoidKeyword },
    { "literal", Token::LiteralKeyword, },

    { "true", Token::TrueKeyword },
    { "false", Token::FalseKeyword },

    // Statements
    { "using", Token::UsingKeyword },
};


const HashTableMap<Token, const char*> eletTokenToString =
{
    // Declarations
    { Token::DomainKeyword, "domain"},
    { Token::FunctionKeyword, "fn" },
    { Token::ClassKeyword, "class"},
    { Token::InterfaceKeyword, "interface" },
    { Token::ExternKeyword, "extern" },
    { Token::AssemblyKeyword, "assembly" },
    { Token::EnumKeyword, "enum" },
    { Token::VarKeyword, "var" },

    // Modifiers / Metadata / Clauses
    { Token::StaticKeyword, "static" },
    { Token::PublicKeyword, "public" },
    { Token::PublicKeyword, "private" },
    { Token::ContextKeyword, "context" },
    { Token::TypesKeyword, "types" },
    { Token::ParamsKeyword, "params" },
    { Token::ImplementsKeyword, "implements" },
    { Token::FromKeyword, "from" },

    // Control flows
    { Token::IfKeyword, "if" },
    { Token::ElseKeyword, "else" },
    { Token::ForKeyword, "for" },
    { Token::WhileKeyword, "while" },
    { Token::ReturnKeyword, "return" },

    // Statements
    { Token::UsingKeyword, "using" },

    // Operators
    { Token::LengthOfKeyword, "lengthof" },

    // Types
    { Token::IntKeyword, "int" },
    { Token::UnsignedIntKeyword, "uint" },
    { Token::USizeKeyword, "usize" },
    { Token::CharKeyword,  "char" },
    { Token::StringKeyword, "string" },
    { Token::VoidKeyword, "void" },
    { Token::LiteralKeyword, "literal" },


    { Token::TrueKeyword,  "true" },
    { Token::FalseKeyword, "false" },

    // For errors only
    { Token::Identifier, "identifier" },
    { Token::DoubleColon, "::" },
    { Token::SemiColon, ";" },
};

#endif //ELET_SCANNER_H
