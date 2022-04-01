#include "ErrorWriter.h"
#include "Domain/Compiler/Error/Error.h"


namespace elet::domain::compiler::ast
{
    ErrorWriter::ErrorWriter()
    {

    }


    void
    ErrorWriter::printCompileErrors(List<error::SyntaxError*>& errors)
    {
        for (const error::SyntaxError* syntaxError : errors)
        {
            writeSyntaxError(syntaxError);
        }
    }


    void
    ErrorWriter::printLexicalErrors(List<error::LexicalError*>& errors)
    {
        for (const error::LexicalError* lexicalError : errors)
        {
            writeLexicalError(lexicalError);
        }
    }


    void
    ErrorWriter::writeLexicalError(const error::LexicalError* lexicalError)
    {
        Utf8StringView source(lexicalError->sourceFile->start, lexicalError->sourceFile->end);
        TextScanner scanner(source);
        _scanner = &scanner;
        unsigned int line = 1;
        unsigned int column = 1;

        while (true)
        {
            Utf8String::Character ch = scanner.getCharacter();
            if (ch == Utf8String::Character::EndOfFile)
            {
                break;
            }
            if (scanner.getPositionAddress() == lexicalError->positionAddress)
            {
                scanner.scanRestOfLine();
                _currentLineEnd = scanner.getPositionAddress();
                break;
            }
            if (scanner.isNewline(ch))
            {
                line++;
                column = 1;
                _currentLineStart = scanner.getPositionAddress() + 1;
            }
            else
            {
                column++;
            }
            scanner.increment();
        }
        writeErrorHeader(lexicalError, line, column);
        writeErrorLine(line);

        Utf8StringView stringView(_currentLineStart, _currentLineEnd);
        Utf8StringView::Iterator iterator = stringView.begin();
        while (iterator.getPositionAddress() != lexicalError->positionAddress)
        {
            _tw.write(" ");
            ++iterator;
        }
        if (lexicalError->endAddress)
        {
            while (iterator.getPositionAddress() != lexicalError->endAddress)
            {
                _tw.write("^");
                ++iterator;
            }
        }
        else
        {
            _tw.write("^");
        }
        _tw.newline();
    }


    void
    ErrorWriter::writeSyntaxError(const error::SyntaxError* syntaxError)
    {
        Utf8StringView source(syntaxError->sourceFile->start, syntaxError->sourceFile->end);
        TextScanner scanner(source);
        _scanner = &scanner;
        unsigned int line = 1;
        unsigned int column = 1;
        _currentLineStart = scanner.getPositionAddress();

        while (true)
        {
            Utf8String::Character ch = scanner.getCharacter();
            if (ch == Utf8String::Character::EndOfFile)
            {
                break;
            }
            if (scanner.getPositionAddress() == syntaxError->syntax->start)
            {
                scanner.scanRestOfLine();
                _currentLineEnd = scanner.getPositionAddress();
                break;
            }
            if (scanner.isNewline(ch))
            {
                line++;
                column = 1;
                _currentLineStart = scanner.getPositionAddress() + 1;
            }
            else
            {
                column++;
            }
            scanner.increment();
        }
        writeErrorHeader(syntaxError, line, column);
        writeErrorLine(line);
        writeUnderlineOnSyntax(syntaxError);
    }


    void
    ErrorWriter::writeErrorLine(unsigned int line)
    {
        int exponent = 1;
        while (true)
        {
            unsigned int base = std::pow(10, exponent);
            if (line / base == 0)
            {
                break;
            }
            exponent++;
        }
        Utf8StringView errorLine(_currentLineStart, _currentLineEnd);
        writeSpaceIndent(exponent);
        _tw.writeLine(" | ");
        _tw.writeUint(line);
        _tw.write(" | ");
        _tw.writeLine(errorLine);
        writeSpaceIndent(exponent);
        _tw.write(" | ");
    }


    void
    ErrorWriter::writeSpaceIndent(unsigned int indentation)
    {
        for (int i = 0; i < indentation; ++i)
        {
            _tw.write(" ");
        }
    }


    void
    ErrorWriter::writeUnderlineOnSyntax(const error::SyntaxError* syntaxError)
    {
        Utf8StringView stringView(_currentLineStart, _currentLineEnd);
        Utf8StringView::Iterator iterator = stringView.begin();
        while (iterator.getPositionAddress() != syntaxError->syntax->start)
        {
            _tw.write(" ");
            ++iterator;
        }
        while (iterator.getPositionAddress() != syntaxError->syntax->end)
        {
            _tw.write("^");
            ++iterator;
        }
        _tw.newline();
    }


    template<typename TError>
    void
    ErrorWriter::writeErrorHeader(TError syntaxError, unsigned int line, unsigned int column)
    {
        _tw.write("error: ");
        _tw.write(syntaxError->message);
        _tw.newline();
        _tw.write("--> ");
        _tw.write(syntaxError->sourceFile->file.string().c_str());
        _tw.write(":");
        _tw.writeUint(line);
        _tw.write(":");
        _tw.writeUint(column);
        _tw.newline();
    }


    Utf8String
    ErrorWriter::toString()
    {
        return _tw.toString();
    }
}