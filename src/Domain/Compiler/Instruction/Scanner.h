#ifndef ELET_ASSEMBLY_X86_SCANNER_H
#define ELET_ASSEMBLY_X86_SCANNER_H

#include <cstddef>
#include <Foundation/Utf8String.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/TextScanner.h>
#include <Foundation/HashTableMap.h>

using namespace elet::foundation;

namespace elet::domain::compiler::instruction
{

class InstructionScanner : public TextScanner
{

public:

    explicit InstructionScanner(const Utf8StringView& source);

    using Character = Utf8String::Character;

    enum class InstructionToken : std::size_t
    {
        Unknown,

        Identifier,
        Variable,

        Comment,

        // Literals
        HexadecimalLiteral,
        BinaryLiteral,
        DecimalLiteral,

        Colon,
        Comma,
        Plus,
        Minus,
        Semicolon,

        Register,

        // Dual Operand
        StartDualOperand,
        Store32Keyword,
        Store64Keyword,
        LeaKeyword,
        EndDualOperand,

        // Singly Operand
        StartSinglyOperand,
        JeKeyword,
        JneKeyword,
        EndSinglyOperand,

        // No Operands
        EndKeyword,

        OpenBracket,
        CloseBracket,

        EndOfFile,
    };

    InstructionToken
    takeNextToken();

    std::size_t
    getRegisterCount() const;

    Utf8StringView
    getAssemblyReferenceValue() const;

private:

    std::size_t
    _registerCount;

    InstructionToken
    getTokenFromString(const Utf8StringView& string) const;

    InstructionToken
    scanBinaryOrHexadecimalLiteral();

    void
    scanDigits();

    void
    scanHexDigits();
};


using InstructionToken = InstructionScanner::InstructionToken;
const HashTableMap<const char*, InstructionToken> IRStringToToken =
{
    { "Store32", InstructionToken::Store32Keyword },
    { "Store64", InstructionToken::Store64Keyword },
    { "End", InstructionToken::EndKeyword },
};


const HashTableMap<InstructionToken, const char*> tokenToIRString =
{
    { InstructionToken::Store64Keyword, "Store64" },
    { InstructionToken::Store32Keyword, "Store32" },
    { InstructionToken::EndKeyword,     "End" },
};


}

#endif //ELET_X86_SCANNER_H
