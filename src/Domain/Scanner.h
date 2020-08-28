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

        Name,

        // Keywords
        UseKeyword,
        ModuleKeyword,
        ImplementKeyword,
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

        // Declarations
        AssemblyKeyword,
        ImportKeyword,
        ExportKeyword,
        FunctionKeyword,
        VarKeyword,
        DomainKeyword,
        ObjectKeyword,
        ConstructorKeyword,
        InterfaceKeyword,
        ContextKeyword,
        StartKeyword,
        EndKeyword,

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

        // Operators
        LengthOfKeyword,
        SizeOfKeyword,

        StringLiteral,

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
};

using Token = Scanner::Token;
const HashTableMap<const char*, Token> eletStringToToken =
{
    { "context", Token::ContextKeyword },
    { "literal", Token::LiteralKeyword, },
    { "domain", Token::DomainKeyword },
    { "var", Token::VarKeyword },
    { "lengthof", Token::LengthOfKeyword },
    { "assembly", Token::AssemblyKeyword },
    { "enum", Token::EnumKeyword },
    { "module", Token::ModuleKeyword },
    { "import", Token::ImportKeyword },
    { "export", Token::ExportKeyword },
    { "use", Token::UseKeyword },
    { "implement", Token::ImplementKeyword },
    { "module", Token::ModuleKeyword },
    { "start", Token::StartKeyword },
    { "end", Token::EndKeyword },
    { "interface", Token::InterfaceKeyword },
    { "object", Token::ObjectKeyword },
    { "constructor", Token::ConstructorKeyword },
    { "fn", Token::FunctionKeyword },
    { "if", Token::IfKeyword },
    { "else", Token::ElseKeyword },
    { "return", Token::ReturnKeyword },
    { "enum", Token::EnumKeyword },
    { "int", Token::IntKeyword },
    { "uint", Token::UnsignedIntKeyword },
    { "usize", Token::USizeKeyword },
    { "char", Token::CharKeyword },
    { "string", Token::StringKeyword },
    { "void", Token::VoidKeyword },
};


const HashTableMap<Token, const char*> eletTokenToString =
{
    { Token::ContextKeyword, "context" },
    { Token::LiteralKeyword, "literal" },
    { Token::DomainKeyword, "domain"},
    { Token::VarKeyword, "var" },
    { Token::LengthOfKeyword, "lengthof" },
    { Token::AssemblyKeyword, "assembly" },
    { Token::EnumKeyword, "enum" },
    { Token::InterfaceKeyword, "interface" },
    { Token::ModuleKeyword, "module" },
    { Token::ImportKeyword, "import" },
    { Token::ExportKeyword, "export" },
    { Token::UseKeyword, "use" },
    { Token::ImplementKeyword, "implement" },
    { Token::ModuleKeyword, "module" },
    { Token::StartKeyword, "start" },
    { Token::EndKeyword , "end" },
    { Token::ObjectKeyword, "object" },
    { Token::ConstructorKeyword, "constructor" },
    { Token::FunctionKeyword, "fn" },
    { Token::IfKeyword, "if" },
    { Token::ElseKeyword,        "else" },
    { Token::ReturnKeyword,      "return" },
    { Token::EnumKeyword,        "enum" },
    { Token::IntKeyword,         "int" },
    { Token::UnsignedIntKeyword, "uint" },
    { Token::USizeKeyword,       "usize" },
    { Token::CharKeyword,        "char" },
    { Token::StringKeyword,      "string" },
    { Token::VoidKeyword,        "void" },

    { Token::Name,               "identifier" },
    { Token::DoubleColon,        "::" },
    { Token::SemiColon,          ";" },
};

#endif //ELET_SCANNER_H
