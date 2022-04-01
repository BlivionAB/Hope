#include "Scanner.h"
#include "Domain/Compiler/Error/Error.h"


namespace elet::domain::compiler::ast
{
    using namespace elet::foundation;


    Scanner::Scanner(const SourceFile* sourceFile):
        TextScanner(Utf8StringView(sourceFile->start, sourceFile->end)),
        _sourceFile(sourceFile)
    {

    }


    Token
    Scanner::takeNextToken(bool includeWhitespaceToken)
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
                    if (includeWhitespaceToken)
                    {
                        return scanWhitespace();
                    }
                    continue;
                case Character::_0:
                {
                    Character ch = getCharacter();
                    if (ch == Character::x)
                    {
                        return scanHexadecimalDigits();
                    }
                    else if (ch == Character::b)
                    {
                        return scanBinaryDigits();
                    }
                    return scanDecimalDigits();
                }
                case Character::_1:
                case Character::_2:
                case Character::_3:
                case Character::_4:
                case Character::_5:
                case Character::_6:
                case Character::_7:
                case Character::_8:
                case Character::_9:
                {
                    return scanDecimalDigits();
                }
                case Character::Hash:
                    return Token::Hash;
                case Character::Comma:
                    return Token::Comma;
                case Character::Asterisk:
                    return Token::Asterisk;
                case Character::ForwardSlash:
                    return Token::ForwardSlash;
                case Character::BackwardSlash:
                    return Token::BackwardSlash;
                case Character::SingleQuote:
                    return scanCharacterLiteral();
                case Character::DoubleQuote:
                    return scanString();
                case Character::Dot:
                    return Token::Dot;
                case Character::Colon:
                    if (getCharacter() == Character::Colon)
                    {
                        increment();
                        return Token::DoubleColon;
                    }
                    return Token::Colon;
                case Character::Semicolon:
                    return Token::SemiColon;
                case Character::OpenBracket:
                    return Token::OpenBracket;
                case Character::CloseBracket:
                    return Token::CloseBracket;
                case Character::OpenParen:
                    return Token::OpenParen;
                case Character::CloseParen:
                    return Token::CloseParen;
                case Character::OpenBrace:
                    return Token::OpenBrace;
                case Character::CloseBrace:
                    return Token::CloseBrace;
                case Character::Ampersand:
                    if (getCharacter() == Character::Ampersand)
                    {
                        increment();
                        return Token::AmpersandAmpersand;
                    }
                    return Token::Ampersand;
                case Character::Plus:
                    return Token::Plus;
                case Character::Minus:
                    return Token::Minus;
                case Character::Caret:
                    return Token::Caret;
                case Character::Pipe:
                    if (getCharacter() == Character::Pipe)
                    {
                        increment();
                        return Token::PipePipe;
                    }
                    return Token::Pipe;
                case Character::Equal:
                    if (getCharacter() == Character::Equal)
                    {
                        increment();
                        return Token::EqualEqual;
                    }
                    return Token::Equal;
                case Character::Percent:
                    return Token::Percent;
                default:
                    if (isIdentifierStart(character))
                    {
                        while (isIdentifierPart(getCharacter()))
                        {
                            increment();
                        }
                        return getTokenFromString(getTokenValue());
                    }
                    return Token::Unknown;
            }
        }
        return Token::EndOfFile;
    }


    Token
    Scanner::getTokenFromString(const Utf8StringView& string) const
    {
        std::optional token = eletStringToToken.find(string);
        if (token)
        {
            if (token == Token::StringKeyword && _stage & TREAT_STRING_KEYWORD_AS_NAME)
            {
                return Token::Identifier;
            }
            return *token;
        }
        return Token::Identifier;
    }


    Utf8StringView
    Scanner::getTokenValue() const
    {
        return _source.slice(_startMemoryLocationOfToken, _sourceIterator.getPositionAddress());
    }


    Token
    Scanner::peekNextToken(bool includeWhitespaceToken)
    {
        saveCurrentLocation();
        Token token = takeNextToken(includeWhitespaceToken);
        revertToSavedLocation();
        return token;
    }


    Token
    Scanner::scanString()
    {
        while (true)
        {
            Character character = getCharacter();
            increment();
            if (character == Character::DoubleQuote || character == Character::EndOfFile)
            {
                break;
            }
        }
        return Token::StringLiteral;
    }

    void
    Scanner::setFlag(std::uint8_t stage)
    {
        _stage = stage;
    }

    void
    Scanner::resetFlags()
    {
        _stage = 0;
    }


    Token
    Scanner::scanDecimalDigits()
    {
        while (true)
        {
            Character character = getCharacter();
            if ((character >= Character::_0 && character <= Character::_9) ||
                character == Character::_)
            {
                increment();
                continue;
            }
            break;
        }
        return Token::DecimalLiteral;
    }


    Token
    Scanner::scanHexadecimalDigits()
    {
        increment();
        bool hasAtLeastOneHexCharacter = false;
        while (true)
        {
            Character character = getCharacter();
            if ((character >= Character::_0 && character <= Character::_9)
                || (character >= Character::a && character <= Character::f))
            {
                hasAtLeastOneHexCharacter = true;
                increment();
                continue;
            }
            else if (character == Character::_)
            {
                increment();
                continue;
            }
            break;
        }
        if (!hasAtLeastOneHexCharacter)
        {
            throwLexicalError("Hexadecimal literal must consist of at least one hexadecimal character.");
        }
        return Token::HexadecimalLiteral;
    }


    Token
    Scanner::scanBinaryDigits()
    {
        increment();
        bool hasAtLeastOneHexCharacter = false;
        while (true)
        {
            Character character = getCharacter();
            if ((character == Character::_0 || character == Character::_1))
            {
                hasAtLeastOneHexCharacter = true;
                increment();
                continue;
            }
            else if (character == Character::_)
            {
                increment();
                continue;
            }
            break;
        }
        if (!hasAtLeastOneHexCharacter)
        {
            throwLexicalError("Binary literal must consist of at least one binary character.");
        }
        return Token::BinaryLiteral;
    }


    Token
    Scanner::scanWhitespace()
    {
        while (true)
        {
            Character character = getCharacter();
            switch (character)
            {
                case Character::Space:
                case Character::Newline:
                case Character::CarriageReturn:
                    increment();
                    continue;
            }
            break;
        }
        return Token::Whitespace;
    }


    Token
    Scanner::scanCharacterLiteral()
    {
        Character character = getCharacter();
        const char* start = getPositionAddress();
        if (character == Character::BackwardSlash)
        {
            increment();
            character = getCharacter();
            increment();
            if (character == Character::x)
            {
                uint32_t result = 0;
                int8_t numberOfDigits = scanHexDigits(result);
                if (numberOfDigits == -1)
                {
                    throwLexicalError("Expected hex digit.");
                }

                // hex escape sequence can accomodate unsigned part.
                if (static_cast<uint32_t>(result) > UCHAR_MAX)
                {
                    throwLexicalError(start, getPositionAddress(), "Character literal must be in ASCII (non-extended) range.");
                }
                _characterLiteralValue = result;
            }
            else {
                switch (character)
                {
                    case Character::n:
                        _characterLiteralValue = static_cast<uint8_t>(Character::Newline);
                        break;
                    case Character::r:
                        _characterLiteralValue = static_cast<uint8_t>(Character::CarriageReturn);
                        break;
                    case Character::t:
                        _characterLiteralValue = static_cast<uint8_t>(Character::Tab);
                        break;
                    case Character::BackwardSlash:
                        _characterLiteralValue = static_cast<uint8_t>(Character::BackwardSlash);
                        break;
                    case Character::_0:
                        _characterLiteralValue = static_cast<uint8_t>(Character::NullCharacter);
                        break;
                    default:
                        throwLexicalError(start, getPositionAddress(), "Unknown escape sequence.");
                }
            }
        }
        else if (static_cast<uint32_t>(character) > CHAR_MAX)
        {
            throwLexicalError("Character literal must be in ASCII (non-extended) range.");
        }
        else
        {
            increment();
            _characterLiteralValue = static_cast<uint8_t>(character);
        }
        character = getCharacter();
        if (character != Character::SingleQuote)
        {
            throwLexicalError("Expected single quote (') character.");
        }
        increment();
        return Token::CharacterLiteral;
    }


    void
    Scanner::throwLexicalError(const char* message) const
    {
        char* positionAddress = getPositionAddress();
        throw new error::LexicalError(_sourceFile, positionAddress, message);
    }


    void
    Scanner::throwLexicalError(const char* startAddress, const char* endAddress, const char* message) const
    {
        throw new error::LexicalError(_sourceFile, startAddress, endAddress, message);
    }


    int8_t
    Scanner::scanHexDigits(uint32_t& result)
    {
        Character character = getCharacter();

        if (character >= Character::_0 && character <= Character::_9)
        {
            increment();
            uint8_t exp = scanHexDigits(result) + 1;
            result += (static_cast<uint32_t>(character) - static_cast<uint32_t>(Character::_0)) * static_cast<uint32_t>(std::pow(16, exp));
            return exp;
        }
        else if (character >= Character::a && character <= Character::f)
        {
            increment();
            uint8_t exp = scanHexDigits(result) + 1;
            uint32_t e = static_cast<uint32_t>(std::pow(16, exp));
            result += (static_cast<uint32_t>(character) - static_cast<uint32_t>(Character::a) + 10) * e;
            return exp;
        }
        else
        {
            return -1;
        }
    }


    uint8_t
    Scanner::getCharacterLiteralValue()
    {
        return _characterLiteralValue;
    }
}
