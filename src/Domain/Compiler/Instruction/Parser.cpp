#include "Parser.h"
#include "Parser.Error.h"

namespace elet::domain::compiler::instruction
{


InstructionParser::InstructionParser(const Utf8StringView &source):
    _scanner(source)
{

}


InstructionToken
InstructionParser::takeNextToken()
{
    return _scanner.takeNextToken();
}


void
InstructionParser::seek(const BaseScanner::Location& location)
{
    _scanner.seek(location);
}


BaseScanner::Location
InstructionParser::getLocation() const
{
    return _scanner.getLocation();
}

EmbeddedSyntax*
InstructionParser::parseStatement()
{
    InstructionToken token = takeNextToken();
    switch (token)
    {
        case InstructionToken::Identifier:
            expectToken(InstructionToken::Colon);
        case InstructionToken::Store32Keyword:
        case InstructionToken::Store64Keyword:
            return parseInstruction(token);
        case InstructionToken::EndKeyword:
        {
            EndStatement *end = createSyntax<EndStatement>(SyntaxKind::EndStatement);
            expectToken(InstructionToken::Semicolon);
            finishSyntax(end);
            return end;
        }
    }

}


void
InstructionParser::expectToken(InstructionToken token)
{
    InstructionToken result = takeNextToken();
    if (result != token)
    {
        throw ExpectedTokenError(token, _scanner.getTokenValue());
    }
}


template<typename T>
T*
InstructionParser::createSyntax(SyntaxKind kind)
{
    T* syntax = new T();
    syntax->kind = kind;
    syntax->start = _scanner.getStartPosition();
    return syntax;
}


EmbeddedInstruction*
InstructionParser::parseInstruction(InstructionToken token)
{
    EmbeddedInstruction* instruction = createInstructionFromToken(token);
    finishSyntax(instruction);
    return instruction;
}


template<typename T>
void
InstructionParser::finishSyntax(T* syntax)
{
    syntax->end = _scanner.getPositionAddress();
}

InstructionType
InstructionParser::getMnemonicKindFromToken(InstructionToken token)
{
    switch (token)
    {
        case InstructionToken::Store32Keyword:
            return InstructionType::Store32;
        case InstructionToken::Store64Keyword:
            return InstructionType::Store64;
    }
}

EmbeddedInstruction*
InstructionParser::createInstructionFromToken(InstructionToken token)
{
    EmbeddedInstruction* instruction = createSyntax<EmbeddedInstruction>(SyntaxKind::EmbeddedInstruction);
    instruction->mnemonic = createMnemonic(token);
    unsigned int numberOfOperands = getNumberOfOperandsFromToken(token);
    if (numberOfOperands > 0)
    {
        instruction->operand1 = parseOperand();
        if (numberOfOperands == 2)
        {
            expectToken(InstructionToken::Comma);
            instruction->operand2 = parseOperand();
        }
        expectToken(InstructionToken::Semicolon);
    }

    return instruction;
}



Mnemonic*
InstructionParser::createMnemonic(InstructionToken token)
{
    Mnemonic* mnemonic = createSyntax<Mnemonic>(SyntaxKind::EmbeddedInstruction);
    mnemonic->type = getMnemonicKindFromToken(token);
    finishSyntax(mnemonic);
    return mnemonic;
}


unsigned int
InstructionParser::getNumberOfOperandsFromToken(InstructionToken token)
{
    if (token > InstructionToken::StartSinglyOperand && token < InstructionToken::EndSinglyOperand)
    {
        return 1;
    }
    if (token > InstructionToken::StartDualOperand && token < InstructionToken::EndDualOperand)
    {
        return 2;
    }
    return 0;
}


embedded::Operand*
InstructionParser::parseOperand()
{
    InstructionToken token = takeNextToken();
    switch (token)
    {
        case InstructionToken::Register:
        {
            embedded::Register* reg = createSyntax<embedded::Register>(SyntaxKind::Register);
            reg->__count = _scanner.getRegisterCount();
            finishSyntax(reg);
            return reg;
        }
        case InstructionToken::DecimalLiteral:
        {
            DecimalLiteral* binary = createSyntax<DecimalLiteral>(SyntaxKind::DecimalLiteral);
            binary->__value = _scanner.getDecimalValue();
            finishSyntax(binary);
            return binary;
        }
        case InstructionToken::HexadecimalLiteral:
        {
            HexadecimalLiteral* hex = createSyntax<HexadecimalLiteral>(SyntaxKind::HexadecimalLiteral);
            hex->__value = _scanner.getHexadecimalValue();
            finishSyntax(hex);
            return hex;
        }
        case InstructionToken::BinaryLiteral:
        {
            BinaryLiteral* binary = createSyntax<BinaryLiteral>(SyntaxKind::BinaryLiteral);
            finishSyntax(binary);
            return binary;
        }
        case InstructionToken::Variable:
        {
            Variable* variable = createSyntax<Variable>(SyntaxKind::Variable);
            variable->__symbol = _scanner.getAssemblyReferenceValue();
            finishSyntax(variable);
            return variable;
        }
    }

    return nullptr;
}

Punctuation*
InstructionParser::createPunctuation(PunctuationType type)
{
    Punctuation* punctuation = createSyntax<Punctuation>(SyntaxKind::Punctuation);
    punctuation->type = type;
    finishSyntax(punctuation);
    return punctuation;
}

List<output::Instruction*>*
InstructionParser::parse()
{
    return nullptr;
//    List<EmbeddedSyntax*> syntaxTree;
//    while (true)
//    {
//        EmbeddedSyntax* statement = parseStatement();
//        if (!statement)
//        {
//            break;
//        }
//        syntaxTree.add(statement);
//        if (statement->kind == SyntaxKind::EndStatement)
//        {
//            break;
//        }
//    }
//    auto list = new List<output::Instruction*>();
//    for (const auto& statement : syntaxTree)
//    {
//        if (statement->kind == SyntaxKind::EmbeddedInstruction)
//        {
//            auto embeddedInstruction = reinterpret_cast<embedded::EmbeddedInstruction*>(statement);
//            auto instruction = new output::Instruction();
//            instruction->type = embeddedInstruction->mnemonic->type;
//            if (embeddedInstruction->operand1)
//            {
//                instruction->operand1 = createOutputOperandFromEmbeddedOperand(embeddedInstruction->operand1);
//            }
//            if (embeddedInstruction->operand2)
//            {
//                instruction->operand2 = createOutputOperandFromEmbeddedOperand(embeddedInstruction->operand2);
//            }
//            list->add(instruction);
//        }
//    }
//    return list;
}

output::Operand*
InstructionParser::createOutputOperandFromEmbeddedOperand(embedded::Operand* operand)
{
    switch (operand->kind)
    {
        case SyntaxKind::Register:
        {
            auto _register = new output::Register(reinterpret_cast<embedded::Register*>(operand)->__count);
            return _register;
        }
        case SyntaxKind::Variable:
            return new output::AssemblyReference(reinterpret_cast<embedded::Variable*>(operand)->__symbol);
        case SyntaxKind::BinaryLiteral:
        case SyntaxKind::DecimalLiteral:
        case SyntaxKind::HexadecimalLiteral:
            return new output::Int32(reinterpret_cast<embedded::NumberLiteral*>(operand)->__value);
        default:
            throw std::exception();
    }
}


}
