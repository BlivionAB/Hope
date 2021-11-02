#ifndef ELET_SOME_SCANNER_H
#define ELET_SOME_SCANNER_H

#include <Foundation/Utf8StringView.h>
#include <Foundation/HashTableMap.h>
#include <Foundation/TextScanner.h>
#include "Syntax/Syntax.h"
#include <stack>

namespace elet::domain::compiler::ast
{
    using namespace elet::foundation;

    #define TREAT_STRING_KEYWORD_AS_NAME (std::uint8_t)0x1

    class Scanner : public elet::foundation::TextScanner
    {
    public:
        explicit Scanner(const SourceFile* source);


        enum class IntegerLiteralType
        {
            Decimal = 10,
            Hexadecimal = 16,
            Binary = 2,
        };


        enum class IntegerSuffix
        {
            None,
            u8,
            u16,
            u32,
            u64,
            s8,
            s16,
            s32,
            s64,
        };

        Token
        peekNextToken(bool includeWhitespaceToken);

        Token
        takeNextToken(bool includeWhitespaceToken);

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

        const
        ast::SourceFile*
        _sourceFile;

        std::uint8_t
        _stage;

        IntegerLiteralType
        _currentIntegerLiteralType;

        Scanner::IntegerSuffix
        _currentIntegerSuffix;

        Utf8StringView
        _sourceString;

        Token
        scanDecimalDigits();

        Token
        scanHexadecimalDigits();

        Token
        scanWhitespace();
    };


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
        { "u8", Token::U8Keyword },
        { "u16", Token::U16Keyword },
        { "u32", Token::U32Keyword },
        { "u64", Token::U64Keyword },
        { "s8", Token::S8Keyword },
        { "s16", Token::S16Keyword },
        { "s32", Token::S32Keyword },
        { "s64", Token::S64Keyword },

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
        { Token::OpenBrace, "{" },
        { Token::CloseBrace, "}" },
        { Token::OpenBracket, "[" },
        { Token::CloseBracket, "]" },
    };
};

#endif //ELET_SCANNER_H
