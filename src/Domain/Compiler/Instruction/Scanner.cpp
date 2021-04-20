#include "Scanner.h"


namespace elet::domain::compiler::instruction
{


using InstructionToken = InstructionScanner::InstructionToken;


InstructionScanner::InstructionScanner(const Utf8StringView &source):
    elet::foundation::BaseScanner(source)
{

}


InstructionToken
InstructionScanner::takeNextToken()
{
    while (_sourceIterator != _endIterator)
    {
        setTokenStartPosition();
        Character character = getCharacter();
        increment();
        switch (character)
        {
            case Character::CarriageReturn:
            case Character::Newline:
            case Character::Space:
                continue;
            case Character::OpenBracket:
                return InstructionToken::OpenBracket;
            case Character::CloseBracket:
                return InstructionToken::CloseBracket;
            case Character::Comma:
                return InstructionToken::Comma;
            case Character::Semicolon:
                return InstructionToken::Semicolon;
            case Character::Plus:
                return InstructionToken::Plus;
            case Character::Minus:
                return InstructionToken::Minus;
            case Character::Slash:
                if (getCharacter() == Character::Slash)
                {
                    increment();
                    scanRestOfLine();
                    continue;
                }
            case Character::_0:
                return scanBinaryOrHexadecimalLiteral();
            case Character::_1:
            case Character::_2:
            case Character::_3:
            case Character::_4:
            case Character::_5:
            case Character::_6:
            case Character::_7:
            case Character::_8:
            case Character::_9:
                scanDigits();
                return InstructionToken::DecimalLiteral;

            case Character::$:
            {
                character = getCharacter();
                if (isIdentifierStart(character))
                {
                    while (isIdentifierPart(getCharacter()))
                    {
                        increment();
                    }
                    return InstructionToken::Variable;
                }
                return InstructionToken::Unknown;
            }
            default:
                if (isIdentifierStart(character))
                {
                    if (character == Character::R)
                    {
                        Character nextCharacter = getCharacter();
                        if (isDigit(nextCharacter))
                        {
                            _registerCount = static_cast<std::size_t>(nextCharacter) - static_cast<std::size_t>(Character::_0);
                            increment();
                            nextCharacter = getCharacter();
                            while (isDigit(nextCharacter))
                            {
                                _registerCount = static_cast<std::size_t>(nextCharacter) - static_cast<std::size_t>(Character::_0);
                                increment();
                                nextCharacter = getCharacter();
                            }
                            return InstructionToken::Register;
                        }
                    }
                    while (isIdentifierPart(getCharacter()))
                    {
                        increment();
                    }
                    return getTokenFromString(getTokenValue());
                }
                return InstructionToken::Unknown;
        }
    }
    return InstructionToken::EndOfFile;
}


InstructionToken
InstructionScanner::getTokenFromString(const Utf8StringView& string) const
{
    std::optional token = IRStringToToken.find(string);
    if (token)
    {
        return *token;
    }
    return InstructionToken::Unknown;
}


InstructionToken
InstructionScanner::scanBinaryOrHexadecimalLiteral()
{
    Character nextCharacter = getCharacter();
    increment();
    if (nextCharacter == Character::x)
    {
        scanHexDigits();
        return InstructionToken::HexadecimalLiteral;
    }
    if (nextCharacter == Character::b)
    {
        scanDigits();
        return InstructionToken::BinaryLiteral;
    }
    return InstructionToken::Unknown;
}


void
InstructionScanner::scanDigits()
{
    Character nextCharacter = getCharacter();
    while (isDigit(nextCharacter))
    {
        increment();
        nextCharacter = getCharacter();
    }
}


void
InstructionScanner::scanHexDigits()
{
    Character nextCharacter = getCharacter();
    while (isHexDigit(nextCharacter))
    {
        increment();
        nextCharacter = getCharacter();
    }
}


std::size_t
InstructionScanner::getRegisterCount() const
{
    return _registerCount;
}


Utf8StringView
InstructionScanner::getAssemblyReferenceValue() const
{
    return _source.slice(_startMemoryLocationOfToken + 1, _sourceIterator.getPosition()).toString();
}


}