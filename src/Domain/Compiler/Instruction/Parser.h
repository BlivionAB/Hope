#ifndef ELET_ASSEMBLY_X86_PARSER_H
#define ELET_ASSEMBLY_X86_PARSER_H

#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include "Scanner.h"
#include "Syntax.h"
#include "Instruction.h"


namespace elet::domain::compiler::instruction
{


namespace output
{
    struct Instruction;
    struct Operand;
}

using namespace foundation;
using namespace embedded;

using InstructionToken = InstructionScanner::InstructionToken;

class InstructionParser
{

public:

    explicit InstructionParser(const Utf8StringView& source);

    void
    seek(const TextScanner::Location& location);

    List<output::Instruction*>*
    parse();

    TextScanner::Location
    getLocation() const;

private:

    InstructionScanner
    _scanner;

    InstructionToken
    takeNextToken();

    template<typename T>
    T*
    createSyntax(embedded::SyntaxKind kind);

    template<typename T>
    void
    finishSyntax(T* syntax);

    EmbeddedInstruction *
    createInstructionFromToken(InstructionToken token);

    EmbeddedSyntax*
    parseStatement();

    void
    expectToken(InstructionToken token);

    EmbeddedInstruction*
    parseInstruction(InstructionToken token);

    static
    InstructionType
    getMnemonicKindFromToken(InstructionToken token);

    Mnemonic*
    createMnemonic(InstructionToken token);

    embedded::Operand*
    parseOperand();

    Punctuation*
    createPunctuation(PunctuationType type);

    static
    unsigned int
    getNumberOfOperandsFromToken(InstructionToken token);

    static
    output::Operand*
    createOutputOperandFromEmbeddedOperand(embedded::Operand* operand);
};


}

#endif //ELET_ASSEMBLY_X86_PARSER_H
