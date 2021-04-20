#ifndef ELET_ASSEMBLY_X86_SYNTAX_H
#define ELET_ASSEMBLY_X86_SYNTAX_H


#include <Foundation/HashTableMap.h>


namespace elet::domain::compiler::instruction::embedded
{


enum class SyntaxKind
{
    EmbeddedInstruction,
    Mnemonic,
    Register,
    Punctuation,
    Variable,
    BinaryLiteral,
    DecimalLiteral,
    HexadecimalLiteral,
    EndStatement,
};


enum class PunctuationType
{
    OpenBracket,
    CloseBracket,
};


enum class InstructionType
{
    Store32,
    Store64,
    StoreAddress32,
    StoreAddress64,
    Call,
    SysCall,
};


const HashTableMap<InstructionType, const char*> instructionTypeToString =
{
    {InstructionType::Store32, "Store32" },
    {InstructionType::Store64, "Store64" },
    {InstructionType::StoreAddress32, "StoreAddress32" },
    {InstructionType::StoreAddress64, "StoreAddress64" },
    {InstructionType::SysCall, "SysCall" },
    {InstructionType::Call,    "Call" },
};


struct EmbeddedSyntax
{
    SyntaxKind
    kind;

    char*
    start;

    char*
    end;
};


struct Punctuation : EmbeddedSyntax
{
    PunctuationType
    type;
};


struct Operand : EmbeddedSyntax
{
    unsigned int
    __count;
};


struct Register : Operand
{

};


struct EndStatement : EmbeddedSyntax
{

};


struct NumberLiteral : Operand
{
    std::uint64_t
    __value;
};

struct HexadecimalLiteral : NumberLiteral
{

};


struct BinaryLiteral : NumberLiteral
{

};


struct DecimalLiteral : NumberLiteral
{

};


struct Variable : Operand
{
    Utf8StringView
    __symbol;
};


struct Expression : EmbeddedSyntax
{

};

struct AddressableOperand : EmbeddedSyntax
{
    Punctuation*
    openBracket;

    Expression*
    expression;

    Punctuation*
    closeBracket;
};


struct Mnemonic : EmbeddedSyntax
{
    Register*
    name;

    InstructionType
    type;
};


struct EmbeddedInstruction : EmbeddedSyntax
{
    Mnemonic*
    mnemonic;

    Operand*
    operand1;

    Operand*
    operand2;
};


}
#endif //ELET_ASSEMBLY_X86_SYNTAX_H
