#ifndef ELET_SOME_SCANNER_H
#define ELET_SOME_SCANNER_H

#include <Foundation/Utf8StringView.h>
#include <Foundation/HashTableMap.h>
#include <Foundation/BaseScanner.h>
#include <stack>

using namespace elet::foundation;


class Scanner : public elet::foundation::BaseScanner
{
public:
    explicit Scanner(const Utf8StringView& source);

    enum class Token : std::size_t
    {
        Unknown,

        Identifier,

        // Keywords
        AssemblyKeyword,
        ImportKeyword,
        ExportKeyword,
        UseKeyword,
        ObjectKeyword,
        InterfaceKeyword,
        ModuleKeyword,
        EndKeyword,
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
        FunctionKeyword,
        VarKeyword,

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
        FloatKeyword,
        DoubleKeyword,

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

private:

    Token
    scanString();

    Token
    getTokenFromString(const Utf8StringView& string) const;
};

using Token = Scanner::Token;
const HashTableMap<const char*, Token> eletStringToToken =
{
    { "var", Token::VarKeyword },
    { "lengthof", Token::LengthOfKeyword },
    { "assembly", Token::AssemblyKeyword },
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
    { "char", Token::CharKeyword },
    { "void", Token::VoidKeyword },
};


const HashTableMap<Token, const char*> eletTokenToString =
{
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
    { Token::EndKeyword , "end" },
    { Token::ObjectKeyword, "object" },
    { Token::FunctionKeyword, "fn" },
    { Token::IfKeyword, "if" },
    { Token::ElseKeyword, "else" },
    { Token::ReturnKeyword, "return" },
    { Token::EnumKeyword, "enum" },
    { Token::IntKeyword, "int" },
    { Token::UnsignedIntKeyword, "uint" },
    { Token::CharKeyword, "char" },
    { Token::VoidKeyword, "void" },

    { Token::Identifier, "identifier" },
    { Token::DoubleColon, "::" },
    { Token::SemiColon, ";" },
};

#endif //ELET_SCANNER_H
