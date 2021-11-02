#ifndef ELET_ERRORWRITER_H
#define ELET_ERRORWRITER_H


#include <Foundation/TextWriter.h>
#include "Exceptions.h"


using namespace elet::foundation;


namespace elet::domain::compiler::ast
{
    struct ErrorLocation
    {
        Utf8StringView
        lineString;

        unsigned int
        line = 1;

        unsigned int
        column = 1;

        ErrorLocation(Utf8StringView lineString):
            lineString(lineString)
        { }
    };


    class ErrorWriter final
    {
    public:

        ErrorWriter();

        void
        printCompileErrors(List<error::SyntaxError*>& errors);

        void
        printLexicalErrors(List<error::LexicalError*>& errors);

        Utf8String
        toString();

    private:

        TextWriter
        _tw;

        TextScanner*
        _scanner;

        const char*
        _currentLineStart;

        const char*
        _currentLineEnd;

        void
        writeSyntaxError(const error::SyntaxError* syntaxError);

        void
        writeUnderlineOnSyntax(const error::SyntaxError* syntaxError);

        void
        writeErrorLine(unsigned int line);

        template<typename TError>
        void
        writeErrorHeader(TError syntaxError, unsigned int line, unsigned int column);

        void
        writeLexicalError(const error::LexicalError* lexicalError);

        void
        writeSpaceIndent(unsigned int indentation);
    };
}


#endif //ELET_ERRORWRITER_H
