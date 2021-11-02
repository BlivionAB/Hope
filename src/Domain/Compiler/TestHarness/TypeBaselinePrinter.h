#ifndef ELET_TYPEBASELINEPRINTER_H
#define ELET_TYPEBASELINEPRINTER_H


#include <Foundation/Utf8String.h>
#include <Foundation/TextWriter.h>
#include <Foundation/TextScanner.h>
#include "../Syntax/Syntax.h"


namespace elet::domain::compiler::test
{
    using namespace foundation;

    struct TypeEntry
    {
        TextScanner::Location
        location;

        ast::Type*
        type;

        size_t
        size;

        TypeEntry(TextScanner::Location location, ast::Type* type, size_t size):
            location(location),
            type(type),
            size(size)
        { }
    };


    class TypeBaselinePrinter
    {
    public:

        Utf8String
        printSourceFiles(const List<ast::SourceFile*>& sourceFile);

    private:

        TextWriter
        _tw;

        ast::SourceFile*
        _sourceFile;

        TextScanner*
        _textScanner;

        const char*
        _cursor;

        void
        writeFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration);

        void
        writeExpression(const ast::Expression* expression, List<TypeEntry>& typeEntries);

        void
        writeIntegerLiteral(const ast::IntegerLiteral* integerLiteral, List<TypeEntry>& typeEntries);

        TextScanner::Location
        writeUntilPositionAddress(const char* positionAddress);

        void
        writeType(ast::Type* type);
    };
}


#endif //ELET_TYPEBASELINEPRINTER_H
