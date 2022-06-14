#include "TypeBaselinePrinter.h"
#include <Foundation/TextScanner.h>

namespace elet::domain::compiler::test
{
    Utf8String
    TypeBaselinePrinter::printSourceFiles(const List<ast::SourceFile*>& sourceFiles)
    {
        for (const ast::SourceFile* sourceFile : sourceFiles)
        {
            _cursor = sourceFile->start;

            Utf8StringView sourceFileStringView(sourceFile->start, sourceFile->end);
            TextScanner textScanner(sourceFileStringView);
            _textScanner = &textScanner;
            for (const auto& iteration : sourceFile->declarations)
            {
                if (iteration.second->kind == ast::SyntaxKind::FunctionDeclaration)
                {
                    writeFunctionDeclaration(reinterpret_cast<ast::FunctionDeclaration*>(iteration.second));
                }
            }
            writeUntilPositionAddress(sourceFile->end);
        }

        return _tw.toString();
    }


    void
    TypeBaselinePrinter::writeFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration)
    {
        for (const ast::Syntax* statement : functionDeclaration->body->statements)
        {
            List<TypeEntry> typeEntries;
            switch (statement->kind)
            {
                case ast::SyntaxKind::ReturnStatement:
                    writeExpression(reinterpret_cast<const ast::ReturnStatement*>(statement)->expression, typeEntries);
                    break;
                case ast::SyntaxKind::VariableDeclaration:
                    writeExpression(reinterpret_cast<const ast::VariableDeclaration*>(statement)->expression, typeEntries);
                    break;
                default:
                    throw std::runtime_error("Not implemented statement yet.");
            }
            _textScanner->scanRestOfLine();
            writeUntilPositionAddress(_textScanner->getPositionAddress());
            for (const TypeEntry& typeEntry : typeEntries)
            {
                _tw.newline();
                for (int c = 1; c < typeEntry.location.column - 1; ++c)
                {
                    _tw.write(" ");
                }
                for (int c = 0; c < typeEntry.size; ++c)
                {
                    _tw.write("~");
                }
                _tw.write("  ");
                writeType(typeEntry.type);
            }
        }
    }


    void
    TypeBaselinePrinter::writeExpression(const ast::Expression* expression, List<TypeEntry>& typeEntries)
    {
        TextScanner::Location startLocation = writeUntilPositionAddress(expression->start);
        TextScanner::Location endLocation = writeUntilPositionAddress(expression->end);
        typeEntries.emplace(startLocation, expression->resultingType, endLocation.column - startLocation.column);
    }


    void
    TypeBaselinePrinter::writeType(ast::Type* type)
    {
        switch (type->kind)
        {
            case TypeKind::U64:
                _tw.write("u64");
                break;
            case TypeKind::U32:
                _tw.write("u32");
                break;
            case TypeKind::U16:
                _tw.write("u16");
                break;
            case TypeKind::U8:
                _tw.write("u8");
                break;
            case TypeKind::S64:
                _tw.write("s64");
                break;
            case TypeKind::S32:
                _tw.write("s32");
                break;
            case TypeKind::S16:
                _tw.write("s16");
                break;
            case TypeKind::S8:
                _tw.write("s8");
                break;
            default:
                throw std::runtime_error("Have not implemented integer type yet.");
        }
        _tw.space();
        writeBounds(type);
    }


    void
    TypeBaselinePrinter::writeBounds(const ast::Type* type)
    {
        if (type->minValue == type->maxValue)
        {
            _tw.write(type->minValue);
        }
        else
        {
            _tw.write(type->minValue);
            _tw.write("..");
            _tw.write(type->maxValue);
        }
    }


    TextScanner::Location
    TypeBaselinePrinter::writeUntilPositionAddress(const char* positionAddress)
    {
        Utf8StringView stringView(_cursor, positionAddress);
        _tw.write(stringView);
        _textScanner->scanToPositionAddress(positionAddress);
        TextScanner::Location location = _textScanner->getLocation();
        _cursor = positionAddress;
        return location;
    }
}
